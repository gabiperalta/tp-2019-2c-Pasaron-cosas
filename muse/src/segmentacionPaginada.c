/*
 * segmentacionPaginada.c
 *
 *  Created on: 5 oct. 2019
 *      Author: utnso
 */

// Funciones para la segmentacion paginada de MUSE

#include "segmentacionPaginada.h"


t_proceso* crear_proceso(char* id_programa,int socket_creado){
	t_proceso* nuevo = malloc(sizeof(t_proceso));

	nuevo->id_programa = strdup(id_programa);
	nuevo->socket = socket_creado;
	nuevo->tabla_segmentos = list_create();

	return nuevo;
	//list_add(lista_threads,nuevo);
}

uint32_t crear_segmento(uint8_t tipo,t_list* tabla_segmentos,uint32_t tam_solicitado) {
	int cantidad_paginas_solicitadas;
	t_heap_metadata heap_metadata;
	uint8_t bit_presencia = 0;
	uint32_t tam_solicitado_real = sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size) + tam_solicitado;
	int posicion = 0;
	void* buffer;
	bool agregar_metadata_free = false;

	t_segmento* nuevo = malloc(sizeof(t_segmento));
	nuevo->tipo_segmento = tipo;
	nuevo->tabla_paginas = list_create();
	nuevo->base = obtener_base(tabla_segmentos);

	printf("Se crea un nuevo segmento\n");

	if(tipo == SEGMENTO_HEAP)
		bit_presencia = 1;

	if((tam_solicitado_real%TAM_PAGINA) != 0){  // si da 0, no necesito agregar la metadata para indicar FREE
		tam_solicitado_real += sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size); //agrego la metadata para indicar FREE
		agregar_metadata_free = true;
	}

	cantidad_paginas_solicitadas = (int)ceil((double)tam_solicitado_real/TAM_PAGINA);
	nuevo->limite = cantidad_paginas_solicitadas * TAM_PAGINA;

	// comienzo a crear el buffer para luego dividirlo en paginas
	buffer = malloc(nuevo->limite);

	heap_metadata.isFree = false;
	heap_metadata.size = tam_solicitado;

	memcpy(&buffer[posicion],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
	posicion += sizeof(heap_metadata.isFree);

	memcpy(&buffer[posicion],&heap_metadata.size,sizeof(heap_metadata.size));
	posicion += sizeof(heap_metadata.size) + heap_metadata.size;

	if(agregar_metadata_free){
		heap_metadata.isFree = true;
		heap_metadata.size = nuevo->limite - posicion - sizeof(heap_metadata.isFree) - sizeof(heap_metadata.size); // revisar despues si es lo mismo q hacer sizeof(t_heap_metadata)

		memcpy(&buffer[posicion],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
		posicion += sizeof(heap_metadata.isFree);

		memcpy(&buffer[posicion],&heap_metadata.size,sizeof(heap_metadata.size));
	}

	cargar_datos(buffer,nuevo->tabla_paginas,CREAR_DATOS,cantidad_paginas_solicitadas);

	list_add(tabla_segmentos,nuevo);

	free(buffer);

	return nuevo->base + sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size);
}

t_pagina* crear_pagina(uint8_t bit_presencia) {
	int frame_obtenido;

	t_pagina* new = malloc(sizeof(t_pagina));
    //new->numeroPagina = numeroPagina;
    new->bit_usado = 0;
    new->bit_modificado = 0;
    new->bit_presencia = bit_presencia;
    frame_obtenido = obtener_frame_libre();

    if(frame_obtenido >= 0){
    	new->frame = frame_obtenido;
    }
    else{
    	// no hay espacio, por lo tanto se tiene que liberar un frame y pasarlo a swap
    	void* buffer = ejecutar_algoritmo_clock_modificado();
    	int frame_swap_obtenido = obtener_frame_swap_libre();

    	archivo_swap = fopen(PATH_SWAP,"r+");

    	fseek(archivo_swap,frame_swap_obtenido*TAM_PAGINA,SEEK_SET);
    	fwrite(buffer,TAM_PAGINA,1,archivo_swap);

    	new->frame = obtener_frame_libre();
    	free(buffer);
    	fclose(archivo_swap);

    	printf("Hasta aca funciona\n");
    }

    return new;
}

t_proceso* buscar_proceso(t_list* lista,int socket_proceso) {
	int igualSocket(t_proceso *p) {
		return p->socket == socket_proceso;
	}

	return list_find(lista, (void*) igualSocket);
}

t_segmento* buscar_segmento(t_list* tabla_segmentos,uint32_t direccion){
	t_segmento* segmento_obtenido;
	for(int i=0;i<list_size(tabla_segmentos);i++){
		segmento_obtenido = list_get(tabla_segmentos,i);
		if((segmento_obtenido->base < direccion) && (segmento_obtenido->limite >= direccion)){
			return segmento_obtenido;
		}
	}

	return NULL;
}

void cargar_datos(void* buffer,t_list* tabla_paginas,uint32_t flag_operacion,int cantidad_paginas_solicitadas){
	t_pagina* pagina;
	void* direccion_frame;
	int paginas_a_recorrer = cantidad_paginas_solicitadas;

	if(paginas_a_recorrer == NULL)
		paginas_a_recorrer = list_size(tabla_paginas);

	for(int numero_pagina=0;numero_pagina<paginas_a_recorrer;numero_pagina++){
		if(flag_operacion == CREAR_DATOS){
			pagina = crear_pagina(1);
			list_add(tabla_paginas,pagina);
	    	printf("Se actualizo la tabla de paginas\n");
		}
		else{
			pagina = list_get(tabla_paginas,numero_pagina);
		}
		direccion_frame = obtener_datos_frame(pagina);

		switch(flag_operacion){
			case CARGAR_DATOS:
				memcpy(&buffer[TAM_PAGINA*numero_pagina],direccion_frame,TAM_PAGINA);
				break;
			case GUARDAR_DATOS:
			case CREAR_DATOS:
				memcpy(direccion_frame,&buffer[TAM_PAGINA*numero_pagina],TAM_PAGINA);
		    	printf("Se copiaron los datos nuevos\n");
				break;
		}
	}
}

void* obtener_datos_frame(t_pagina* pagina){
	// Tambien deberia chequear si el frame se encuentra en memoria o no
	return (char*) upcm + (pagina->frame * TAM_PAGINA);
}

int obtener_frame_libre(){
	for(int i=0;i<cantidad_frames;i++){
		if(!bitarray_test_bit(bitmap_upcm,i)){ // retorna el primer bit q encuentre en 0
			bitarray_set_bit(bitmap_upcm,i);
			return i;
		}
	}
	// si no retorno, no hay frames libres, por lo tanto reviso si hay espacio libre en swap
	/*
	if(espacio_en_swap()){
		ejecutar_algoritmo_clock_modificado();
	}
	else{
		return -1; // no hay frames libres
	}
	*/
	return -1; // no hay frames libres
}

int obtener_frame_swap_libre(){
	for(int i=0;i<cantidad_frames_swap;i++){
		if(!bitarray_test_bit(bitmap_swap,i)){ // retorna el primer bit q encuentre en 0
			bitarray_set_bit(bitmap_swap,i);
			return i;
		}
	}
	return -1; // no hay frames libres
}

uint32_t obtener_base(t_list* tabla_segmentos){
	// obtengo el ultimo segmento
	if(list_size(tabla_segmentos)){
		t_segmento* segmento_obtenido = list_get(tabla_segmentos,list_size(tabla_segmentos) - 1);
		return segmento_obtenido->base + segmento_obtenido->limite;
	}
	return 0;
}

int filtrarHeap(t_segmento* p){
	return p->tipo_segmento == SEGMENTO_HEAP;
}

void liberar_frame(int numero_frame){
	bitarray_clean_bit(bitmap_upcm,numero_frame);
}

// me va a retornar los datos de la pagina que se libera de la memoria principal
void* ejecutar_algoritmo_clock_modificado(){
	printf("Se ejecuta el algoritmo clock modificado\n");
	t_proceso* proceso_obtenido;
	t_segmento* segmento_obtenido;
	t_pagina* pagina_obtenida;
	int primer_frame_recorrido = algoritmo_clock_frame_recorrido;
	int nro_vuelta = 1;
	void* datos_pagina;

	while(true){
		for(int nro_proceso=0;nro_proceso<list_size(lista_procesos);nro_proceso++){
			proceso_obtenido = list_get(lista_procesos,nro_proceso);
			printf("Recorriendo proceso %d\n",nro_proceso);
			sleep(3);
			for(int nro_segmento=0;nro_segmento<list_size(proceso_obtenido->tabla_segmentos);nro_segmento++){
				segmento_obtenido = list_get(proceso_obtenido->tabla_segmentos,nro_segmento);
				printf("Recorriendo segmento %d\n",nro_segmento);
				sleep(3);
				for(int nro_pagina=0;nro_pagina<list_size(segmento_obtenido->tabla_paginas);nro_pagina++){
					pagina_obtenida = list_get(segmento_obtenido->tabla_paginas,nro_pagina);
					printf("Recorriendo pagina %d\n",nro_pagina);
					sleep(3);
					if((algoritmo_clock_frame_recorrido == pagina_obtenida->frame) && pagina_obtenida->bit_presencia){
						switch(nro_vuelta){
							case 1:
								if(!pagina_obtenida->bit_usado && !pagina_obtenida->bit_modificado){
									datos_pagina = malloc(TAM_PAGINA);
									memcpy(datos_pagina,&upcm[algoritmo_clock_frame_recorrido*TAM_PAGINA],TAM_PAGINA);
									liberar_frame(algoritmo_clock_frame_recorrido);
									algoritmo_clock_frame_recorrido++;
									return datos_pagina;
								}
								break;
							case 2:
								if(!pagina_obtenida->bit_usado && pagina_obtenida->bit_modificado){
									//envio la pagina a swap
									datos_pagina = malloc(TAM_PAGINA);
									memcpy(datos_pagina,&upcm[algoritmo_clock_frame_recorrido*TAM_PAGINA],TAM_PAGINA);
									liberar_frame(algoritmo_clock_frame_recorrido);
									algoritmo_clock_frame_recorrido++;
									return datos_pagina;
								}
								else{
									pagina_obtenida->bit_usado = 0;
								}
								break;
						}

						algoritmo_clock_frame_recorrido++;
						if(algoritmo_clock_frame_recorrido == cantidad_frames)
							algoritmo_clock_frame_recorrido = 0;
						if(algoritmo_clock_frame_recorrido == primer_frame_recorrido){
							nro_vuelta++;
							if(nro_vuelta>2){
								nro_vuelta = 1;
							}
						}
					}
				}
			}
		}
	}
	//list_iterate(lista_procesos,(void*) analizar_segmentos);
}

void eliminar_pagina(t_pagina* pagina){
	if(pagina->bit_presencia){
		memset(obtener_datos_frame(pagina),NULL,TAM_PAGINA);
	}
	else{
		// liberar del swap
	}
	liberar_frame(pagina->frame);
	free(pagina);
}
