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
	t_heap_metadata heap_metadata;
	SIZE_HEAP_METADATA = sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size);

	/////////////////////// UPCM ///////////////////////
	cantidad_frames = TAM_MEMORIA / TAM_PAGINA;
	int cantidad_frames_bytes = (int)ceil((double)cantidad_frames/8);
	void* frames_bitmap = malloc(cantidad_frames_bytes);
	memset(frames_bitmap,NULL,cantidad_frames_bytes); // seteo frames_bitmap en 0 por las dudas

	upcm = malloc(TAM_MEMORIA); // memoria principal
	bitmap_upcm = bitarray_create_with_mode(frames_bitmap,cantidad_frames_bytes,MSB_FIRST);
	////////////////////////////////////////////////////

	/////////////////////// SWAP ///////////////////////
	cantidad_frames_swap = TAM_SWAP / TAM_PAGINA;
	int cantidad_frames_swap_bytes = (int)ceil((double)cantidad_frames_swap/8);
	void* frames_swap_bitmap = malloc(cantidad_frames_swap_bytes);
	memset(frames_swap_bitmap,NULL,cantidad_frames_swap_bytes);

	bitmap_swap = bitarray_create_with_mode(frames_swap_bitmap,cantidad_frames_swap_bytes,MSB_FIRST);

	archivo_swap = fopen(PATH_SWAP,"w");
	fclose(archivo_swap);
	////////////////////////////////////////////////////

	algoritmo_clock_frame_recorrido = 0;
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

	pthread_mutex_lock(&mutex_lista_procesos);
	list_add(lista_procesos,crear_proceso(id_programa,socket_muse));
	pthread_mutex_unlock(&mutex_lista_procesos);

	//				PRUEBA

	t_proceso* proceso_obtenido;
	for(int i=0; i<list_size(lista_procesos); i++){
		proceso_obtenido = list_get(lista_procesos,i);
		printf("proceso nro %d\t",i);
		printf("id_programa: %s\t",proceso_obtenido->id_programa);
		printf("socket: %d\t\n",proceso_obtenido->socket);
	}
	printf("\n");


	free(ip_socket); // Sacar si falla

	t_paquete paquete_respuesta = {
			.header = MUSE_INIT,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros,1); // solo para confirmar que la comunicacion fue exitosa
	enviar_paquete(paquete_respuesta,socket_muse);
	///////////////////////////////////////////////////////
}

void funcion_close(t_paquete paquete,int socket_muse){

}

void funcion_alloc(t_paquete paquete,int socket_muse){
	uint32_t tam = obtener_valor(paquete.parametros);

	pthread_mutex_lock(&mutex_acceso_upcm);
	//pthread_mutex_lock(&mutex_lista_procesos);
	t_proceso* proceso_encontrado = buscar_proceso(lista_procesos,socket_muse);
	//pthread_mutex_unlock(&mutex_lista_procesos);

	t_segmento* segmento_obtenido;
	t_segmento* segmento_siguiente;
	uint32_t direccion_retornada = NULL;
	int posicion_recorrida = 0;
	t_heap_metadata heap_metadata;
	void* buffer;
	uint32_t tam_real = tam + SIZE_HEAP_METADATA;
	uint32_t size_original;
	bool analizar_extension = true;

	bool agregar_metadata_free = true;
	int cantidad_paginas_solicitadas;
	uint32_t segmento_limite_anterior;

	if(proceso_encontrado == NULL){
		printf("No se inicializo libmuse\n");
		return;
	}

	//pthread_mutex_lock(&mutex_acceso_upcm);

	printf("id_programa: %s\t",proceso_encontrado->id_programa);
	printf("socket: %d\t\n",proceso_encontrado->socket);

	for(int numero_segmento=0;numero_segmento<list_size(proceso_encontrado->tabla_segmentos);numero_segmento++){
		segmento_obtenido = list_get(proceso_encontrado->tabla_segmentos,numero_segmento);
		if(segmento_obtenido->tipo_segmento == SEGMENTO_MMAP)
			continue;

		buffer = malloc(segmento_obtenido->limite);// si falla, probar declarando la variable aca mismo
		posicion_recorrida = 0;

		printf("Se cargan los datos en un buffer\n");
		cargar_datos(buffer,segmento_obtenido,CARGAR_DATOS,NULL);

		while(posicion_recorrida < segmento_obtenido->limite){
			memcpy(&heap_metadata.isFree,&buffer[posicion_recorrida],sizeof(heap_metadata.isFree));
			posicion_recorrida += sizeof(heap_metadata.isFree);
			memcpy(&heap_metadata.size,&buffer[posicion_recorrida],sizeof(heap_metadata.size));

			//////// PRUEBA ////////
			printf("metadata\t");
			if(heap_metadata.isFree){
				printf("libre\t %d bytes\n",heap_metadata.size);
			}
			else{
				printf("usado\t %d bytes\n",heap_metadata.size);
			}
			////////////////////////

			if((heap_metadata.isFree == true) && ((heap_metadata.size == tam) || (heap_metadata.size >= tam_real))){
				// hay un espacio libre donde se puede asignar el tam solicitado pero tambien se debe indicar el espacio libre restante
				size_original = heap_metadata.size;
				heap_metadata.isFree = false;
				heap_metadata.size = tam;

				// se cambia la metadata
				posicion_recorrida -= sizeof(heap_metadata.isFree);
				memcpy(&buffer[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
				posicion_recorrida += sizeof(heap_metadata.isFree);
				memcpy(&buffer[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));
				posicion_recorrida += sizeof(heap_metadata.size);

				direccion_retornada = posicion_recorrida + segmento_obtenido->base;

				if(size_original != heap_metadata.size){
					// se agrega la metadata nueva
					posicion_recorrida += heap_metadata.size;

					heap_metadata.isFree = true;
					heap_metadata.size = size_original - tam_real;

					memcpy(&buffer[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
					posicion_recorrida += sizeof(heap_metadata.isFree);
					memcpy(&buffer[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));
					posicion_recorrida += sizeof(heap_metadata.size);
				}

				//posicion_recorrida += heap_metadata.size;
				analizar_extension = false;

				// se vuelven a copiar los datos en los frames correspondientes
				cargar_datos(buffer,segmento_obtenido,GUARDAR_DATOS,NULL);

				free(buffer);
				break;
			}

			posicion_recorrida += sizeof(heap_metadata.size) + heap_metadata.size;
		}

		//se analiza si se puede extender el segmento
		if(!analizar_extension)
			break; // salgo del ciclo for que recorre los segmentos

		// si heap_metadata.isFree es false
		uint32_t tam_auxiliar = tam + SIZE_HEAP_METADATA;
		// tam_auxiliar es el tamano restante que se guardaria en la nueva pagina

		// se revisa cual fue la ultima metadata
		if(heap_metadata.isFree == true){
			tam_auxiliar = tam - heap_metadata.size;
		}

		if((tam_auxiliar%TAM_PAGINA) != 0){  // si da 0, no necesito agregar la metadata para indicar FREE
			tam_auxiliar += SIZE_HEAP_METADATA; //agrego la metadata para indicar FREE
			agregar_metadata_free = true;
		}

		cantidad_paginas_solicitadas = (int)ceil((double)tam_auxiliar/TAM_PAGINA);

		printf("cantidad paginas solicitadas: %d\n",cantidad_paginas_solicitadas);

		//en teoria, si el ultimo segmento es heap, se extiende ese segmento
		if((numero_segmento + 1) < list_size(proceso_encontrado->tabla_segmentos)){
			segmento_siguiente = list_get(proceso_encontrado->tabla_segmentos,numero_segmento + 1);

			// si no hay espacio entre los segmentos, se corta la iteracion actual para analizar el proximo segmento
			if(!((segmento_siguiente->base - (segmento_obtenido->base + segmento_obtenido->limite)) >= (cantidad_paginas_solicitadas*TAM_PAGINA))){
				continue;
			}
		}

		//se puede agrandar el segmento
		t_pagina* pagina_nueva;
		void* buffer_auxiliar = malloc(cantidad_paginas_solicitadas * TAM_PAGINA);
		void* direccion_datos_auxiliar;

		printf("Se extiende el segmento\n");

		//modifico la ultima metadata
		heap_metadata.isFree = false;
		posicion_recorrida = segmento_obtenido->limite - heap_metadata.size - sizeof(heap_metadata.size) - sizeof(heap_metadata.isFree);
		memcpy(&buffer[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
		posicion_recorrida += sizeof(heap_metadata.isFree);
		heap_metadata.size = tam;
		memcpy(&buffer[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));
		posicion_recorrida += sizeof(heap_metadata.size);

		direccion_retornada = posicion_recorrida + segmento_obtenido->base;

		printf("direccion_retornada: %d\n",direccion_retornada);

		posicion_recorrida += heap_metadata.size;
		posicion_recorrida -= segmento_obtenido->limite;

		// se vuelven a copiar los datos en los frames correspondientes
		cargar_datos(buffer,segmento_obtenido,GUARDAR_DATOS,NULL);

		segmento_limite_anterior = segmento_obtenido->limite;
		segmento_obtenido->limite += (cantidad_paginas_solicitadas * TAM_PAGINA);

		if(agregar_metadata_free){
			heap_metadata.isFree = true;
			heap_metadata.size = segmento_obtenido->limite - segmento_limite_anterior - posicion_recorrida - SIZE_HEAP_METADATA; // revisar despues si es lo mismo q hacer sizeof(t_heap_metadata)

			memcpy(&buffer_auxiliar[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
			posicion_recorrida += sizeof(heap_metadata.isFree);

			memcpy(&buffer_auxiliar[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));
		}

		// se copian los datos en los nuevos frames
		printf("Copiando datos en los nuevos frames\n");
		cargar_datos(buffer_auxiliar,segmento_obtenido,CREAR_DATOS,cantidad_paginas_solicitadas);

		free(buffer_auxiliar);
		free(buffer);
	}// termina for

	//si no hay ningun segmento creado, se crea uno nuevo
	if(direccion_retornada == NULL)
		direccion_retornada = crear_segmento(SEGMENTO_HEAP,proceso_encontrado->tabla_segmentos,tam);

	pthread_mutex_unlock(&mutex_acceso_upcm);

	/////////////// PRUEBA ///////////////
	t_segmento* segmento_mostrado;
	t_pagina* pagina_mostrada;
	for(int s=0; s<list_size(proceso_encontrado->tabla_segmentos); s++){
		segmento_mostrado = list_get(proceso_encontrado->tabla_segmentos,s);
		printf("segmento nro %d\t",s);
		printf("tipo: %d\t",segmento_mostrado->tipo_segmento);
		printf("base: %d\t",segmento_mostrado->base);
		printf("limite: %d\t\n",segmento_mostrado->limite);

		printf("tabla de paginas: \n");
		for(int p=0; p<list_size(segmento_mostrado->tabla_paginas); p++){
			pagina_mostrada = list_get(segmento_mostrado->tabla_paginas,p);
			printf("pagina nro %d\t",p);
			printf("bit presencia: %d\t",pagina_mostrada->bit_presencia);
			printf("frame: %d\t\n",pagina_mostrada->frame);
		}
	}
	printf("\n");
	//////////////////////////////////////

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
	printf("Inicio free\n");
	uint32_t direccion_recibida = obtener_valor(paquete.parametros);

	// revisar si la busqueda de proceso se puede hacer directamente en procesar solicitud
	pthread_mutex_lock(&mutex_acceso_upcm);
	//pthread_mutex_lock(&mutex_lista_procesos);
	t_proceso* proceso_encontrado = buscar_proceso(lista_procesos,socket_muse);
	//pthread_mutex_unlock(&mutex_lista_procesos);

	//pthread_mutex_lock(&mutex_acceso_upcm);
	// cuidado con esto, si se elimina proceso_encontrado, pierdo la referencia a _todo???
	t_segmento* segmento_obtenido = buscar_segmento(proceso_encontrado->tabla_segmentos,direccion_recibida);
	// si no se encuentra el segmento, deberia controlar el error
	void* buffer = malloc(segmento_obtenido->limite);
	t_heap_metadata heap_metadata;
	t_heap_metadata heap_metadata_anterior;
	int posicion_recorrida = 0;

	printf("Hasta aca funciona\n");
	printf("id_programa: %s\t",proceso_encontrado->id_programa);
	printf("socket: %d\t\n",proceso_encontrado->socket);
	printf("\n");

	cargar_datos(buffer,segmento_obtenido,CARGAR_DATOS,NULL);

	posicion_recorrida = direccion_recibida - segmento_obtenido->base - SIZE_HEAP_METADATA;
	memcpy(&heap_metadata.isFree,&buffer[posicion_recorrida],sizeof(heap_metadata.isFree));
	//posicion_recorrida += sizeof(heap_metadata.isFree);

	if(heap_metadata.isFree == false){
		// compruebo si el espacio que voy a liberar fue asignado anteriormente, o sea, hubo un muse_alloc antes

	}

	heap_metadata.isFree = true;
	memcpy(&buffer[posicion_recorrida],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
	posicion_recorrida += sizeof(heap_metadata.isFree);
	memcpy(&heap_metadata.size,&buffer[posicion_recorrida],sizeof(heap_metadata.size));
	posicion_recorrida += sizeof(heap_metadata.size);

	memset(&buffer[posicion_recorrida],NULL,heap_metadata.size);

	// comienzo a revisar _todo el segmento para ver si tengo que unificar espacios libres
	posicion_recorrida = 0;
	heap_metadata_anterior.isFree = false; // con esto indico q aun no se inicializo
	while(posicion_recorrida < segmento_obtenido->limite){
		memcpy(&heap_metadata.isFree,&buffer[posicion_recorrida],sizeof(heap_metadata.isFree));
		posicion_recorrida += sizeof(heap_metadata.isFree);
		memcpy(&heap_metadata.size,&buffer[posicion_recorrida],sizeof(heap_metadata.size));

		//////// PRUEBA ////////
		printf("metadata\t");
		if(heap_metadata.isFree){
			printf("libre\t %d bytes\n",heap_metadata.size);
		}
		else{
			printf("usado\t %d bytes\n",heap_metadata.size);
		}
		////////////////////////

		if((heap_metadata_anterior.isFree == true) && (heap_metadata.isFree == true)){
			printf("La metadata actual y la anterior estan libres\n");
			posicion_recorrida -= sizeof(heap_metadata.isFree);
			memset(&buffer[posicion_recorrida],NULL,SIZE_HEAP_METADATA);
			printf("posicion_recorrida : %d \n",posicion_recorrida);

			printf("heap_metadata_anterior.size : %d \n",heap_metadata_anterior.size);
			posicion_recorrida = posicion_recorrida - heap_metadata_anterior.size - sizeof(heap_metadata.size);
			printf("posicion_recorrida : %d \n",posicion_recorrida);
			heap_metadata.size += heap_metadata_anterior.size + SIZE_HEAP_METADATA;
			memcpy(&buffer[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));

			printf("Nuevo espacio libre: %d bytes\n",heap_metadata.size);
		}

		printf("Igualo la metadata a la variable anterior\n");
		heap_metadata_anterior.isFree = heap_metadata.isFree;
		heap_metadata_anterior.size = heap_metadata.size;

		posicion_recorrida += sizeof(heap_metadata.size) + heap_metadata.size;
		printf("posicion_recorrida : %d \n",posicion_recorrida);
	}

	int cantidad_paginas_libres = (int)floor((double)heap_metadata.size/TAM_PAGINA);
	if((heap_metadata.isFree == true) && (cantidad_paginas_libres>0)){
		// analizo si el ultimo espacio esta libre para eliminar las paginas que no
		// esten siendo usadas

		printf("Elimino paginas libres\n");

		int cantidad_paginas_actual = list_size(segmento_obtenido->tabla_paginas);

		for(int pagina_recorrida = cantidad_paginas_actual - 1; pagina_recorrida >= (cantidad_paginas_actual - cantidad_paginas_libres); pagina_recorrida--){
			// hay 1 o mas paginas libres sin usar
			// no hace falta hacer memset ya que se hizo al momento de realizar el free
			list_remove_and_destroy_element(segmento_obtenido->tabla_paginas,pagina_recorrida,(void*) eliminar_pagina);
			segmento_obtenido->limite -= TAM_PAGINA;
		}

		printf("heap_metadata.size: %d\n",heap_metadata.size);

		heap_metadata.size -= cantidad_paginas_libres*TAM_PAGINA;
		posicion_recorrida = segmento_obtenido->limite - heap_metadata.size - sizeof(heap_metadata.size);

		printf("Nuevo heap_metadata.size: %d\n",heap_metadata.size);
		printf("posicion_recorrida: %d\n",posicion_recorrida);
		memcpy(&buffer[posicion_recorrida],&heap_metadata.size,sizeof(heap_metadata.size));
	}

	//sleep(5);
	cargar_datos(buffer,segmento_obtenido,GUARDAR_DATOS,NULL);

	pthread_mutex_unlock(&mutex_acceso_upcm);

	free(buffer);

	/////////////// PRUEBA ///////////////
	t_segmento* segmento_mostrado;
	t_pagina* pagina_mostrada;
	for(int s=0; s<list_size(proceso_encontrado->tabla_segmentos); s++){
		segmento_mostrado = list_get(proceso_encontrado->tabla_segmentos,s);
		printf("segmento nro %d\t",s);
		printf("tipo: %d\t",segmento_mostrado->tipo_segmento);
		printf("base: %d\t",segmento_mostrado->base);
		printf("limite: %d\t\n",segmento_mostrado->limite);

		printf("tabla de paginas: \n");
		for(int p=0; p<list_size(segmento_mostrado->tabla_paginas); p++){
			pagina_mostrada = list_get(segmento_mostrado->tabla_paginas,p);
			printf("pagina nro %d\t",p);
			printf("bit presencia: %d\t",pagina_mostrada->bit_presencia);
			printf("frame: %d\t\n",pagina_mostrada->frame);
		}
	}
	printf("\n");
	//////////////////////////////////////

	t_paquete paquete_respuesta = {
			.header = MUSE_FREE,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	agregar_valor(paquete_respuesta.parametros,1);
	enviar_paquete(paquete_respuesta,socket_muse);
	///////////////////////////////////////////////////////

	printf("Fin free\n");
}

void funcion_get(t_paquete paquete,int socket_muse){

}

void funcion_cpy(t_paquete paquete,int socket_muse){
	char* string_recibido = obtener_string(paquete.parametros);

	printf("%s\n",string_recibido);
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
