/*
 * funciones.c
 *
 *  Created on: 3 oct. 2019
 *      Author: utnso
 */

// Funciones generales de MUSE

#include "funcionesMuse.h"

// El socket que se crea en MUSE es diferente al creado en libmuse
// Ej.: para una misma conexion, MUSE tiene el socket 2 y libmuse el 7

void procesar_solicitud(void* socket_cliente){
	t_paquete paquete = recibir_paquete(socket_cliente);
	void (*funcion_muse)(t_paquete,int);

	// por cada conexion nueva, agrego al proceso/hilo a la lista de threads

	while(paquete.error != 1){
		switch(paquete.header){
			case MUSE_INIT:
				funcion_muse = funcion_init;
				break;
			case MUSE_ALLOC:
				funcion_muse = funcion_alloc;
				break;
			case MUSE_FREE:
				funcion_muse = funcion_free;
				break;
			case MUSE_GET:
				funcion_muse = funcion_get;
				break;
			case MUSE_CPY:
				funcion_muse = funcion_cpy;
				break;
			case MUSE_MAP:
				funcion_muse = funcion_map;
				break;
			case MUSE_SYNC:
				funcion_muse = funcion_sync;
				break;
			case MUSE_UNMAP:
				funcion_muse = funcion_sync;
				break;
			case MUSE_CLOSE:
				// Nunca ingresara a esta condicion, o si
				return;
		}

		funcion_muse(paquete,socket_cliente);

		paquete = recibir_paquete(socket_cliente);
	}

	close(socket_cliente);
}

void leer_config(){
	t_config* archivo_config = config_create(PATH_CONFIG);
	PUERTO = config_get_int_value(archivo_config,"LISTEN_PORT");
	TAM_MEMORIA = config_get_int_value(archivo_config,"MEMORY_SIZE");
	TAM_PAGINA = config_get_int_value(archivo_config,"PAGE_SIZE");
	TAM_SWAP = config_get_int_value(archivo_config,"SWAP_SIZE");
	config_destroy(archivo_config);
}

void init_memoria(){
	cantidad_frames = TAM_MEMORIA / TAM_PAGINA;
	int cantidad_frames_bytes = (int)ceil((double)cantidad_frames/8);
	void* frames_bitmap = malloc(cantidad_frames_bytes);
	memset(frames_bitmap,NULL,cantidad_frames_bytes); // seteo frames_bitmap en 0 por las dudas

	upcm = malloc(TAM_MEMORIA); // memoria principal
	bitmap_upcm = bitarray_create_with_mode(frames_bitmap,cantidad_frames_bytes,MSB_FIRST);
}

void init_threads(){
	pthread_t thread_servidor;
	pthread_create(&thread_servidor,NULL,(void*)servidor,NULL);
	pthread_detach(thread_servidor);
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


void funcion_init(t_paquete paquete,int socket_muse){

	char* id_programa = string_new();
	char* ip_socket = obtener_ip_socket(socket_muse);
	string_append(&id_programa,string_itoa(obtener_valor(paquete.parametros)));
	string_append(&id_programa,"-");
	string_append(&id_programa,ip_socket);

	list_add(lista_procesos,crear_proceso(id_programa,socket_muse));

	//				PRUEBA
	/*
	t_thread* thread_obtenido;
	for(int i=0; i<list_size(lista_threads); i++){
		thread_obtenido = list_get(lista_threads,i);
		printf("hilo nro %d\t",i);
		printf("id_programa: %s\t",thread_obtenido->id_programa);
		printf("socket: %d\t\n",thread_obtenido->socket);
	}
	printf("\n");
	*/

	free(ip_socket); // Sacar si falla
}

void funcion_close(t_paquete paquete,int socket_muse){

}

void funcion_alloc(t_paquete paquete,int socket_muse){
	uint32_t tam = obtener_valor(paquete.parametros);

	t_proceso* proceso_encontrado = buscar_proceso(lista_procesos,socket_muse);
	t_segmento* segmento_obtenido;
	t_pagina* pagina_obtenida;
	uint32_t direccion_retornada;
	void* direccion_frame;
	int posicion_recorrida = 0;
	t_heap_metadata heap_metadata;
	void* buffer;

	if(proceso_encontrado == NULL){
		printf("No se inicializo libmuse\n");
		return;
	}

	t_list* tabla_segmentos_heap = list_filter(proceso_encontrado->tabla_segmentos,(void*) filtrarHeap);

	for(int numero_segmento=0;numero_segmento<list_size(tabla_segmentos_heap);numero_segmento++){
		segmento_obtenido = list_get(tabla_segmentos_heap,numero_segmento);
		buffer = malloc(segmento_obtenido->limite);// si falla, probar declarando la variable aca mismo
		posicion_recorrida = 0;

		for(int numero_pagina=0;numero_pagina<list_size(segmento_obtenido->tabla_paginas);numero_pagina++){
			pagina_obtenida = list_get(segmento_obtenido->tabla_paginas,numero_pagina);

			direccion_frame = obtener_datos_frame(pagina_obtenida);

			memcpy(&buffer[TAM_PAGINA*numero_pagina],direccion_frame,TAM_PAGINA);
		}

		while(posicion_recorrida < segmento_obtenido->limite){
			memcpy(&heap_metadata.isFree,&buffer[posicion_recorrida],sizeof(heap_metadata.isFree));
			posicion_recorrida += sizeof(heap_metadata.isFree);
			memcpy(&heap_metadata.size,&buffer[posicion_recorrida],sizeof(heap_metadata.size));
			posicion_recorrida += sizeof(heap_metadata.size) + heap_metadata.size;

			if((heap_metadata.isFree == true) && (heap_metadata.size <= tam)){

			}
		}

		free(buffer);
	}


	//si no hay ningun segmento creado, se crea uno nuevo
	direccion_retornada = crear_segmento(SEGMENTO_HEAP,proceso_encontrado->tabla_segmentos,tam);


	//t_paquete paquete_respuesta = {
	//		.header = MUSE_ALLOC,
	//		.parametros = list_create()
	//};

	//agregar_valor(paquete_respuesta.parametros,tam_obtenido);
}

void funcion_free(t_paquete paquete,int socket_muse){

}

void funcion_get(t_paquete paquete,int socket_muse){

}

void funcion_cpy(t_paquete paquete,int socket_muse){

}

void funcion_map(t_paquete paquete,int socket_muse){

}

void funcion_sync(t_paquete paquete,int socket_muse){

}

void funcion_unmap(t_paquete paquete,int socket_muse){

}


char* obtener_ip_socket(int s){
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	char* ipstr_reservado = malloc(sizeof(ipstr));
	//int port;

	len = sizeof addr;
	getpeername(s, (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    //port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
	    //port = ntohs(s->sin6_port);
	    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}

	strcpy(ipstr_reservado,ipstr);

	return ipstr_reservado;
}
