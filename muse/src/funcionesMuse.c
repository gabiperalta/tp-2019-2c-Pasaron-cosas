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
	t_segmento* segmento_siguiente;
	t_pagina* pagina_obtenida;
	uint32_t direccion_retornada = NULL;
	void* direccion_frame;
	int posicion_recorrida = 0;
	t_heap_metadata heap_metadata;
	void* buffer;
	uint32_t tam_real = tam + sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size);
	uint32_t size_original;
	uint32_t base_siguiente;
	bool analizar_extension = true;

	bool agregar_metadata_free = true;
	int cantidad_paginas_solicitadas;

	if(proceso_encontrado == NULL){
		printf("No se inicializo libmuse\n");
		return;
	}

	//t_list* tabla_segmentos_heap = list_filter(proceso_encontrado->tabla_segmentos,(void*) filtrarHeap);

	for(int numero_segmento=0;numero_segmento<list_size(proceso_encontrado->tabla_segmentos);numero_segmento++){
		segmento_obtenido = list_get(proceso_encontrado->tabla_segmentos,numero_segmento);
		if(segmento_obtenido->tipo_segmento == SEGMENTO_MMAP)
			continue;

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

			if((heap_metadata.isFree == true) && (heap_metadata.size == tam)){
				// hay un espacio libre donde se puede asignar el tam solicitado
				heap_metadata.isFree = false;
				heap_metadata.size = tam;

				// se cambia la metadata
				posicion_recorrida -= sizeof(heap_metadata.isFree);
				memcpy(&buffer[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
				posicion_recorrida += sizeof(heap_metadata.isFree);
				memcpy(&buffer[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));
				posicion_recorrida += sizeof(heap_metadata.size);

				direccion_retornada = posicion_recorrida + segmento_obtenido->base;

				//posicion_recorrida += heap_metadata.size;
				analizar_extension = false;
				break;
			}
			else if((heap_metadata.isFree == true) && (heap_metadata.size >= tam_real )){
				// hay un espacio libre donde se puede asignar el tam solicitado pero tambien se debe indicar el espacio libre restante
				size_original = heap_metadata.size;
				heap_metadata.isFree = false;
				heap_metadata.size = tam;

				// se cambia la metadata
				posicion_recorrida -= sizeof(heap_metadata.isFree);
				memcpy(&buffer[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
				posicion_recorrida += sizeof(heap_metadata.isFree);
				memcpy(&buffer[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));
				posicion_recorrida += sizeof(heap_metadata.size) + heap_metadata.size;

				heap_metadata.isFree = true;
				heap_metadata.size = size_original - tam_real;

				// se agrega la metadata nueva
				memcpy(&buffer[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
				posicion_recorrida += sizeof(heap_metadata.isFree);
				memcpy(&buffer[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));
				posicion_recorrida += sizeof(heap_metadata.size);

				direccion_retornada = posicion_recorrida + segmento_obtenido->base;

				//posicion_recorrida += heap_metadata.size;
				analizar_extension = false;
				break;
			}

			posicion_recorrida += sizeof(heap_metadata.size) + heap_metadata.size;
		}


		//se analiza si se puede extender el segmento
		if(!analizar_extension)
			break; // salgo del ciclo for que recorre los segmentos

		// si heap_metadata.isFree es false
		uint32_t tam_auxiliar = tam + sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size);
		// tam_auxiliar es el tamano restante que se guardaria en la nueva pagina

		// se revisa cual fue la ultima metadata
		if(heap_metadata.isFree == true){
			tam_auxiliar = tam - heap_metadata.size;
		}

		if((tam_auxiliar%TAM_PAGINA) != 0){  // si da 0, no necesito agregar la metadata para indicar FREE
			tam_auxiliar += sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size); //agrego la metadata para indicar FREE
			agregar_metadata_free = true;
		}

		cantidad_paginas_solicitadas = (int)ceil((double)tam_auxiliar/TAM_PAGINA);

		//en teoria, si el ultimo segmento es heap, se extiende ese segmento
		if((numero_segmento + 1) < list_size(proceso_encontrado->tabla_segmentos)){
			segmento_siguiente = list_get(proceso_encontrado->tabla_segmentos,numero_segmento + 1);

			// si no hay entre los segmentos, se corta la iteracion actual para analizar el proximo segmento
			if(!((segmento_siguiente->base - (segmento_obtenido->base + segmento_obtenido->limite)) >= (cantidad_paginas_solicitadas*TAM_PAGINA))){
				continue;
			}
		}

		//se puede agrandar el segmento
		t_pagina* pagina_nueva;
		void* buffer_auxiliar = malloc(cantidad_paginas_solicitadas * TAM_PAGINA);
		void* direccion_datos_auxiliar;

		//modifico la ultima metadata
		heap_metadata.isFree = false;
		posicion_recorrida = segmento_obtenido->limite - heap_metadata.size - sizeof(heap_metadata.size);
		memcpy(&buffer[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
		posicion_recorrida += sizeof(heap_metadata.isFree);
		heap_metadata.size = tam;
		memcpy(&buffer[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));

		direccion_retornada = posicion_recorrida + sizeof(heap_metadata.size) + segmento_obtenido->base;

		posicion_recorrida -= segmento_obtenido->limite;

		// se vuelven a copiar los datos en los frames correspondientes
		for(int x=0;x<list_size(segmento_obtenido->tabla_paginas);x++){
			pagina_obtenida = list_get(segmento_obtenido->tabla_paginas,x);

			direccion_frame = obtener_datos_frame(pagina_obtenida);

			memcpy(direccion_frame,&buffer[TAM_PAGINA*x],TAM_PAGINA);
		}

		segmento_obtenido->limite += (cantidad_paginas_solicitadas * TAM_PAGINA);

		if(agregar_metadata_free){
			heap_metadata.isFree = true;
			heap_metadata.size = segmento_obtenido->limite - posicion_recorrida - sizeof(heap_metadata.isFree) - sizeof(heap_metadata.size); // revisar despues si es lo mismo q hacer sizeof(t_heap_metadata)

			memcpy(&buffer_auxiliar[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
			posicion_recorrida += sizeof(heap_metadata.isFree);

			memcpy(&buffer_auxiliar[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));
		}

		// se copian los datos en los nuevos frames
		for(int z=0;z<cantidad_paginas_solicitadas;z++){
			pagina_nueva = crear_pagina(1);
			list_add(segmento_obtenido->tabla_paginas,pagina_nueva);

			direccion_datos_auxiliar = obtener_datos_frame(pagina_nueva);

			memcpy(direccion_datos_auxiliar,&buffer_auxiliar[TAM_PAGINA*z],TAM_PAGINA);
		}
		//segmento_obtenido->limite += (cantidad_paginas_solicitadas * TAM_PAGINA);


		free(buffer_auxiliar);
		free(buffer);
	}// termina for


	//si no hay ningun segmento creado, se crea uno nuevo
	if(direccion_retornada == NULL)
		direccion_retornada = crear_segmento(SEGMENTO_HEAP,proceso_encontrado->tabla_segmentos,tam);


	t_paquete paquete_respuesta = {
			.header = MUSE_ALLOC,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros,direccion_retornada);
	enviar_paquete(paquete_respuesta,socket_muse);
	///////////////////////////////////////////////////////
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
