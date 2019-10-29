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

t_segmento* crear_segmento(uint8_t tipo) {
	t_segmento* nuevo = malloc(sizeof(t_segmento));
	nuevo->tipo_segmento = tipo;
	nuevo->tabla_paginas = list_create();
	return nuevo;
}

void crear_segmento_v2(uint8_t tipo,t_list* tabla_segmentos,uint32_t tam_solicitado) {
	int cantidad_paginas_solicitadas;
	//double cantidad_paginas_solicitadas_coma;
	t_heap_metadata heap_metadata;
	uint8_t bit_presencia = 0;
	uint32_t tam_solicitado_real = sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size) + tam_solicitado;
	t_pagina* pagina_nueva;
	void* direccion_datos;
	int posicion = 0;
	t_segmento* nuevo = malloc(sizeof(t_segmento));
	nuevo->tipo_segmento = tipo;
	nuevo->tabla_paginas = list_create();
	nuevo->base = obtener_base(tabla_segmentos);

	if(tipo == SEGMENTO_HEAP)
		bit_presencia = 1;

	if((tam_solicitado_real%TAM_PAGINA) != 0){  // si da 0, no necesito agregar la metadata para indicar FREE
		tam_solicitado_real += sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size); //agrego la metadata para indicar FREE
	}

	cantidad_paginas_solicitadas = (int)ceil((double)tam_solicitado_real/TAM_PAGINA);

	heap_metadata.isFree = false;
	heap_metadata.size = tam_solicitado;

	while(cantidad_paginas_solicitadas > 0){
		pagina_nueva = crear_pagina(bit_presencia);
		list_add(nuevo->tabla_paginas,pagina_nueva);

		direccion_datos = obtener_datos_frame(pagina_nueva);

		memcpy(&direccion_datos[posicion],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
		posicion += sizeof(heap_metadata.isFree);

		cantidad_paginas_solicitadas--;
	}
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

/*
void reservar_espacio(t_thread* thread_solicitante,uint32_t tam,uint8_t tipo_segmento){
	t_list* tabla_segmentos_filtrada;
	t_segmento* segmento_obtenido;
	t_pagina* pagina_obtenida;
	void* direccion_datos;
	t_desplazamiento desplazamiento_obtenido;

	switch(tipo_segmento){
		case SEGMENTO_HEAP:

			tabla_segmentos_filtrada = list_filter(thread_solicitante->tabla_segmentos,(void*) filtrarHeap);

			for(int i=0; i<list_size(tabla_segmentos_filtrada); i++){
				segmento_obtenido = list_get(tabla_segmentos_filtrada,i);
				desplazamiento_obtenido = buscar_bloque_libre(segmento_obtenido->tabla_paginas,tam);

				if(desplazamiento_obtenido != NULL)
					break; //se asigna el bloque
			}



			break;
		case SEGMENTO_MMAP:
			break;
	}

}
*/

t_desplazamiento buscar_bloque_libre(t_list* tabla_paginas,uint32_t tam){
	t_pagina* pagina_obtenida;
	void* direccion_datos;
	int posicion = 0;
	int bytes_recorridos = 0;
	//t_desplazamiento desplazamiento;
	t_heap_metadata heap_metadata;
	int tam_pagina_limite = TAM_PAGINA - sizeof(heap_metadata.isFree) - sizeof(heap_metadata.size);

	for(int x=0; x<list_size(tabla_paginas); x++){
		pagina_obtenida = list_get(tabla_paginas,x);
		direccion_datos = obtener_datos_frame(pagina_obtenida);

		posicion = bytes_recorridos;

		do{
			memcpy(&heap_metadata.isFree,&direccion_datos[posicion],sizeof(heap_metadata.isFree));
			posicion += sizeof(heap_metadata.isFree);
			memcpy(&heap_metadata.size,&direccion_datos[posicion],sizeof(heap_metadata.size));
			posicion += sizeof(heap_metadata.size);

			if(heap_metadata.isFree && (tam<=heap_metadata.size)){
				t_desplazamiento desplazamiento = {
						.numero_pagina = x,
						.posicion = posicion - sizeof(heap_metadata.isFree) - sizeof(heap_metadata.size) // devuelvo la posicion desde la metadata
				};
				return desplazamiento;
			}
			posicion += heap_metadata.size;
		}while(posicion <= tam_pagina_limite);

		bytes_recorridos = posicion - TAM_PAGINA;
	}

	return NULL;
}

/*
void asignar_bloque(t_segmento segmento,t_desplazamiento desplazamiento,uint32_t tam){
	int numero_pagina_recorrida = desplazamiento.numero_pagina;
	t_pagina* pagina_obtenida;
	void* direccion_datos;
	int posicion;
	t_heap_metadata heap_metadata_anterior;

	t_heap_metadata heap_metadata_nuevo = {
			.isFree = false,
			.size = tam
	};

	for(int i=desplazamiento.numero_pagina; i<list_size(segmento.tabla_paginas); i++){
		pagina_obtenida = list_get(segmento.tabla_paginas,numero_pagina_recorrida);
		direccion_datos = obtener_datos_frame(pagina_obtenida);

		posicion = desplazamiento.posicion;

		do{
			memcpy(&heap_metadata_anterior.isFree,&direccion_datos[posicion],sizeof(heap_metadata_anterior.isFree));
			posicion += sizeof(heap_metadata_anterior.isFree);
			memcpy(&heap_metadata_anterior.size,&direccion_datos[posicion],sizeof(heap_metadata_anterior.size));

			posicion -= sizeof(heap_metadata_anterior.isFree);
			memset(&direccion_datos[posicion],NULL,sizeof(heap_metadata_anterior.size) + sizeof(heap_metadata_anterior.isFree));

			memcpy(&direccion_datos[posicion],&heap_metadata_nuevo.isFree,sizeof(heap_metadata_nuevo.isFree));
			posicion += sizeof(heap_metadata_nuevo.isFree);
			memcpy(&direccion_datos[posicion],&heap_metadata_nuevo.size,sizeof(heap_metadata_nuevo.size));
			posicion += sizeof(heap_metadata_nuevo.size) ;

			if(heap_metadata.isFree && (tam<=heap_metadata.size)){
				t_desplazamiento desplazamiento = {
						.numero_pagina = x,
						.posicion = posicion - sizeof(heap_metadata.isFree) - sizeof(heap_metadata.size) // devuelvo la posicion de la metadata
				};
				return desplazamiento;
			}
			posicion += heap_metadata.size;
		}while(posicion <= tam_pagina_limite);

		bytes_recorridos = posicion - TAM_PAGINA;

	}
}
*/

void* obtener_datos_frame(t_pagina* pagina){
	// Tambien deberia chequear si el frame se encuentra en memoria o no
	int posicion_inicio_frame = pagina->frame * TAM_PAGINA;
	return (char*) upcm + posicion_inicio_frame;
}

int obtener_frame_libre(){
	for(int i=0;i<cantidad_frames;i++){
		if(!bitarray_test_bit(bitmap_upcm,i)) // retorna el primer bit q encuentre en 0
			bitarray_set_bit(bitmap_upcm,i);
			return i;
	}
	return -1; // no hay frames libres
}

uint32_t obtener_base(t_list* tabla_segmentos){
	// obtengo el ultimo segmento
	t_segmento* segmento_obtenido = list_get(tabla_segmentos,list_size(tabla_segmentos) - 1);
	return segmento_obtenido->base + segmento_obtenido->limite;
}

int filtrarHeap(t_segmento* p){
	return p->tipo_segmento == SEGMENTO_HEAP;
}
