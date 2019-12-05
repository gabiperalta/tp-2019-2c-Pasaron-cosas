#include "servidor.h"



void inicializarServidor(){

	printf("socketDeEscucha: %i\n", PUERTO);
	pthread_t hiloServidor;

	pthread_create(&hiloServidor,NULL,(void*)servidor, NULL);
	pthread_detach(hiloServidor);
}

void servidor(){
	void * conectado;
	int puerto_escucha = escuchar(PUERTO);

	while((conectado = aceptarConexion(puerto_escucha)) != 1 ){

		//printf("Se acepto conexion\n");
		pthread_t thread_solicitud;
		pthread_create(&thread_solicitud,NULL,(void*)procesar_solicitud,conectado);
		pthread_detach(thread_solicitud);
	}
}


void procesar_solicitud(void *socket_cliente){

	t_paquete paquete = recibir_paquete(socket_cliente);
	void (*funcion_fuse)(t_paquete,int);

	funcion_init(socket_cliente);

	while(paquete.error != 1){
		switch(paquete.header){
		case FUSE_GETATTR:
			funcion_fuse = funcion_getattr;
			printf("FUSE_GETATTR \n");
			break;
		case FUSE_READDIR:
			funcion_fuse = funcion_readdir;
			printf("FUSE_READDIR \n");
			break;
		case FUSE_MKNOD:
			funcion_fuse = funcion_mknod;
			printf("FUSE_MKNOD \n");
			break;
		case FUSE_OPEN:
			funcion_fuse = funcion_open;
			printf("FUSE_OPEN \n");
			break;
		case FUSE_WRITE:
			funcion_fuse = funcion_write;
			printf("FUSE_WRITE \n");
			break;
		case FUSE_READ:
			funcion_fuse = funcion_read;
			printf("FUSE_READ \n");
			break;
		case FUSE_UNLINK:
			funcion_fuse = funcion_unlink;
			printf("FUSE_UNLINK \n");
			break;
		case FUSE_MKDIR:
			funcion_fuse = funcion_mkdir;
			printf("FUSE_MKDIR \n");
			break;
		case FUSE_RMDIR:
			funcion_fuse = funcion_rmdir;
			printf("FUSE_RMDIR \n");
			break;
		}

		funcion_fuse(paquete, socket_cliente);

		paquete = recibir_paquete(socket_cliente);
	}

	funcion_finish(socket_cliente);

	close(socket_cliente);

	return;
}

void funcion_init(/*t_paquete paquete,*/ int socket_fuse){

	ProcessTableNode* nodoListaProcesosAbiertos = malloc(sizeof(ProcessTableNode));
	nodoListaProcesosAbiertos->socket = socket_fuse;
	nodoListaProcesosAbiertos->archivos_abiertos = list_create();

	list_add(listaDeTablasDeArchivosPorProceso, nodoListaProcesosAbiertos);
}

void funcion_finish(int socket_fuse){

	printf("FINALIZANDO PROCESO\n");

	bool buscadorDeNodo(ProcessTableNode* unNodo){
		return unNodo->socket == socket_fuse;
	}

	void destructorTableNode(ProcessTableNode* unNodo){
		list_destroy_and_destroy_elements(unNodo->archivos_abiertos, (void*)free);
		free(unNodo);
	}

	list_remove_and_destroy_by_condition(listaDeTablasDeArchivosPorProceso, (void*)buscadorDeNodo, (void*)destructorTableNode);

	printf("TERMINAMOS DE FINALIZAR EL PROCESO\n");
}

void funcion_getattr(t_paquete paquete,int socket_fuse){

	char* path = obtener_string(paquete.parametros);

	struct stat statRetorno;

	int retorno = myGetattr(path, &statRetorno);

	t_paquete paquete_respuesta = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	agregar_valor(paquete_respuesta.parametros, retorno);

	if( retorno == 0 ){
		if(statRetorno.st_nlink == 2){
			agregar_valor(paquete_respuesta.parametros, statRetorno.st_nlink);
			agregar_valor(paquete_respuesta.parametros, statRetorno.st_mode);
		}
		if(statRetorno.st_nlink == 1){
			agregar_valor(paquete_respuesta.parametros, statRetorno.st_nlink);
			agregar_valor(paquete_respuesta.parametros, statRetorno.st_mode);
			agregar_valor(paquete_respuesta.parametros, statRetorno.st_size);
		}
	}

	///////////////// Parametros a enviar /////////////////
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

	free(path);

}

void funcion_readdir(t_paquete paquete,int socket_fuse){

	char* path = obtener_string(paquete.parametros);


	char* buffer = myReaddir( path ); // TODO, TENGO QUE VERO COMO HAGO CON EL FILLER, SI COMO LE DIJE A JULI O DE OTRA FORMA

	printf("BUFFER: %s\n", buffer);

	t_paquete paquete_respuesta = {
			.header = FUSE_READDIR,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	if(buffer){
		int retorno = 0;
		agregar_valor(paquete_respuesta.parametros, retorno);
		agregar_string(paquete_respuesta.parametros, buffer);
	}
	else{
		int retorno = -1;
		agregar_valor(paquete_respuesta.parametros, retorno);
	}

	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

	free(path);
	// free(buffer);

}

void funcion_mknod(t_paquete paquete,int socket_fuse){

	char* path = obtener_string(paquete.parametros);

	int retorno = crearArchivo( path );

	t_paquete paquete_respuesta = {
			.header = FUSE_MKNOD,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

	free(path);

}


void funcion_open(t_paquete paquete,int socket_fuse){

	char* path = obtener_string(paquete.parametros);

	int retorno = abrirArchivo(path, socket_fuse);

	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

	free(path);

}


void funcion_write(t_paquete paquete,int socket_fuse){
	char* path = obtener_string(paquete.parametros);
	char* buffer = obtener_string(paquete.parametros);
	size_t size = obtener_valor(paquete.parametros);
	off_t offset = obtener_valor(paquete.parametros);

	printf("ESTOY POR ESCRIBIR EN EL ARCHIVO\n");

	int retorno = escribirArchivo(path, buffer, size, offset);

	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

	free(path);
	free(buffer);

}


void funcion_read(t_paquete paquete,int socket_fuse){

	char* path = obtener_string(paquete.parametros);
	char* buffer = obtener_string(paquete.parametros);
	size_t size = obtener_valor(paquete.parametros);
	off_t offset = obtener_valor(paquete.parametros);


	int retorno = leerArchivo(path, buffer, size, offset);

	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};


	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	agregar_string(paquete_respuesta.parametros, buffer);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

	free(path);
	free(buffer);

}


void funcion_unlink(t_paquete paquete,int socket_fuse){

	char* path = obtener_string(paquete.parametros);

	int retorno = eliminarArchivo(path);

	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

	free(path);

}


void funcion_mkdir(t_paquete paquete,int socket_fuse){

	char* path = obtener_string(paquete.parametros);

	printf("MKDIR: Me llego el path: %s \n", path);

	int retorno = crearDirectorio(path);

	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

	free(path);

}


void funcion_rmdir(t_paquete paquete,int socket_fuse){

	char* path = obtener_string(paquete.parametros);

	printf("---RMDIR: Me llego el path: %s \n", path);

	int retorno = eliminarDirectorio(path);

	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

	free(path);

}
