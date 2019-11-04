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
	t_pagina* pagina_nueva;
	void* direccion_datos;
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

	for(int z=0;z<cantidad_paginas_solicitadas;z++){
		pagina_nueva = crear_pagina(bit_presencia);
		list_add(nuevo->tabla_paginas,pagina_nueva);

		direccion_datos = obtener_datos_frame(pagina_nueva);

		memcpy(direccion_datos,&buffer[TAM_PAGINA*z],TAM_PAGINA);
	}

	list_add(tabla_segmentos,nuevo);

	free(buffer);

	return nuevo->base + sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size);
}

t_pagina* crear_pagina(uint8_t bit_presencia) {
    t_pagina* new = malloc(sizeof(t_pagina));
    //new->numeroPagina = numeroPagina;
    //new->bit_modificado = bit_modificado;
    new->bit_presencia = bit_presencia;
    new->frame = obtener_frame_libre();

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
