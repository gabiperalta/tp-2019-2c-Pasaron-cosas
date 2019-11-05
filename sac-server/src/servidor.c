
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

	while((conectado=aceptarConexion(puerto_escucha))!= 1){

		//printf("Se acepto conexion\n");
		pthread_t thread_solicitud;
		pthread_create(&thread_solicitud,NULL,(void*)procesar_solicitud,conectado);
		pthread_detach(thread_solicitud);
	}
}


void procesar_solicitud(void *socket_cliente){

	t_paquete paquete = recibir_paquete(socket_cliente);
	void (*funcion_fuse)(t_paquete,int);


	while(paquete.error != 1){
		switch(paquete.header){
		case FUSE_INIT:
			funcion_fuse = funcion_init;
			break;
		case FUSE_GETATTR:
			funcion_fuse = funcion_getattr;
			break;
		case FUSE_READDIR:
			funcion_fuse = funcion_readdir;
			break;
		case FUSE_MKNOD:
			funcion_fuse = funcion_mknod;
			break;
		case FUSE_OPEN:
			funcion_fuse = funcion_open;
			break;
		case FUSE_WRITE:
			funcion_fuse = funcion_write;
			break;
		case FUSE_READ:
			funcion_fuse = funcion_read;
			break;
		case FUSE_UNLINK:
			funcion_fuse = funcion_unlink;
			break;
		case FUSE_MKDIR:
			funcion_fuse = funcion_mkdir;
			break;
		case FUSE_RMDIR:
			funcion_fuse = funcion_rmdir;
			break;
		}

		funcion_muse(paquete, socket_cliente);

		paquete = recibir_paquete(socket_cliente);
	}

	close(socketCliente);

	return;
}

funcion_init(t_paquete paquete,int socket_fuse){

	ProcessFdNode* nodoListaProcesosAbiertos = malloc(sizeof(ProcessFdNode));
	nodoListaProcesosAbiertos->socket = socket_fuse;
	nodoListaProcesosAbiertos->archivos_abiertos = list_create;

	list_add(listaDeTablasDeArchivosPorProceso, nodoListaProcesosAbiertos);

	t_paquete paquete_respuesta = {
			.header = FUSE_INIT,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros,1); // solo para confirmar que la comunicacion fue exitosa
	enviar_paquete(paquete_respuesta,socket_fuse);
	///////////////////////////////////////////////////////

}
funcion_getattr(t_paquete paquete,int socket_muse){

	char* path = obtener_string(paquete.parametros);
	struct stat statRetorno = obtener_string(paquete.parametros);

	int retorno = myGetattr(path, statRetorno);

	t_paquete paquete_respuesta = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	agregar_string(paquete_respuesta.parametros, statRetorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

}
funcion_readdir(t_paquete paquete,int socket_muse){

	char* path = obtener_string(paquete.parametros);
	char* buffer = obtener_string(paquete.parametros);
	off_t offset = obtener_valor(paquete.parametros);


	int retorno = myReaddir(path, buffer, filler, offset, fi);

	t_paquete paquete_respuesta = {
			.header = FUSE_READDIR,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////


}
funcion_mknod(t_paquete paquete,int socket_muse){

	char* path = obtener_string(paquete.parametros);
	mode_t modo = obtener_valor(paquete.parametros);
	dev_t dev = obtener_valor(paquete.parametros);

	int retorno = crearArchivo(path, modo, dev);

	t_paquete paquete_respuesta = {
			.header = FUSE_MKNOD,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////
}
funcion_open(t_paquete paquete,int socket_muse){

	char* path = obtener_string(paquete.parametros);
	int info = obtener_valor(paquete.parametros);

	int retorno = abrirArchivo(path, info);

	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

}
funcion_write(t_paquete paquete,int socket_muse){

}
funcion_read(t_paquete paquete,int socket_muse){

	char* path = obtener_string(paquete.parametros);


	leerArchivo(path, buffer, size, offset, fi);

	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

}
funcion_unlink(t_paquete paquete,int socket_muse){


	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

}
funcion_mkdir(t_paquete paquete,int socket_muse){


	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

}
funcion_rmdir(t_paquete paquete,int socket_muse){



	t_paquete paquete_respuesta = {
			.header = FUSE_OPEN,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros, retorno);
	enviar_paquete(paquete_respuesta, socket_fuse);
	///////////////////////////////////////////////////////

}
