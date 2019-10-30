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
	void* variable_partida;
	int tam_pagina_restante = TAM_PAGINA;
	int tipo_dato = 0;
	void* buffer;
	bool agregar_metadata_free = false;
	uint32_t limite;
	t_segmento* nuevo = malloc(sizeof(t_segmento));
	nuevo->tipo_segmento = tipo;
	nuevo->tabla_paginas = list_create();
	nuevo->base = obtener_base(tabla_segmentos);

	if(tipo == SEGMENTO_HEAP)
		bit_presencia = 1;

	if((tam_solicitado_real%TAM_PAGINA) != 0){  // si da 0, no necesito agregar la metadata para indicar FREE
		tam_solicitado_real += sizeof(heap_metadata.isFree) + sizeof(heap_metadata.size); //agrego la metadata para indicar FREE
		agregar_metadata_free = true;
	}

	cantidad_paginas_solicitadas = (int)ceil((double)tam_solicitado_real/TAM_PAGINA);
	limite = cantidad_paginas_solicitadas * TAM_PAGINA;
	// comienzo a crear el buffer para luego dividirlo en paginas
	buffer = malloc(limite);

	heap_metadata.isFree = false;
	heap_metadata.size = tam_solicitado;

	memcpy(&buffer[posicion],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
	posicion += sizeof(heap_metadata.isFree);

	memcpy(&buffer[posicion],&heap_metadata.size,sizeof(heap_metadata.size));
	posicion += sizeof(heap_metadata.size) + heap_metadata.size;

	if(agregar_metadata_free){
		heap_metadata.isFree = true;
		heap_metadata.size = limite - posicion - sizeof(heap_metadata.isFree) - sizeof(heap_metadata.size); // revisar despues si es lo mismo q hacer sizeof(t_heap_metadata)

		memcpy(&buffer[posicion],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
		posicion += sizeof(heap_metadata.isFree);

		memcpy(&buffer[posicion],&heap_metadata.size,sizeof(heap_metadata.size));
		posicion += sizeof(heap_metadata.size) + heap_metadata.size;
	}
	// terminar

	while(cantidad_paginas_solicitadas > 0){
		pagina_nueva = crear_pagina(bit_presencia);
		list_add(nuevo->tabla_paginas,pagina_nueva);

		direccion_datos = obtener_datos_frame(pagina_nueva);

		switch(tipo_dato){
			case 0: // heap_metadata.isFree
				tam_pagina_restante -= sizeof(heap_metadata.isFree);
				if(tam_pagina_restante >= 0){
					// ejemplo 1: 50 - 1 = 49
					// ejemplo 2: 50 - 1 = 49 	se solicita 63 bytes
					memcpy(&direccion_datos[posicion],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
					posicion += sizeof(heap_metadata.isFree);
				}
				else{
					// ejemplo: 7 - 10 = -3
					variable_partida = &heap_metadata.isFree;
					memcpy(&direccion_datos[posicion],&variable_partida[0],TAM_PAGINA);
					posicion += sizeof(heap_metadata.isFree);

					continue;
				}
				tipo_dato++;
				break;
			case 1: // heap_metadata.size
				break;
			case 2: // tam solicitado
				break;
			case 3:	// heap_metadata.isFree
				break;
			case 4: // heap_metadata.size
				break;
		}



		// si el tam restante es menor al valor q quiero guardar
		tam_pagina_restante -= sizeof(heap_metadata.isFree);
		if(tam_pagina_restante >= 0){
			// ejemplo 1: 50 - 1 = 49
			// ejemplo 2: 50 - 1 = 49 	se solicita 63 bytes
			memcpy(&direccion_datos[posicion],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
			posicion += sizeof(heap_metadata.isFree);
		}
		else{
			// ejemplo: 7 - 10 = -3
			variable_partida = &heap_metadata.isFree;
			memcpy(&direccion_datos[posicion],&variable_partida[0],TAM_PAGINA);
			posicion += sizeof(heap_metadata.isFree);

			continue;
		}

		tam_pagina_restante -= sizeof(heap_metadata.size);
		if(tam_pagina_restante >= 0){
			// ejemplo 1: 49 - 4 = 45
			// ejemplo 2: 49 - 4 = 45
			memcpy(&direccion_datos[posicion],&heap_metadata.size,sizeof(heap_metadata.size));
			posicion += sizeof(heap_metadata.size);
		}
		else{
			// ejemplo: 7 - 10 = -3
			variable_partida = &heap_metadata.size;
			memcpy(&direccion_datos[posicion],&variable_partida[0],TAM_PAGINA);
			posicion += sizeof(heap_metadata.size);

			continue;
		}

		tam_pagina_restante -= heap_metadata.size;
		if(tam_pagina_restante > 0){
			// ejemplo 1: 45 - 10 = 35
			heap_metadata.isFree = false;
			tam_pagina_restante -= sizeof(heap_metadata.isFree);
			if(tam_pagina_restante >= 0){
				// ejemplo 1: 35 - 1 = 34
				memcpy(&direccion_datos[posicion],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
				posicion += sizeof(heap_metadata.isFree);
			}
			else{
				// ejemplo: 7 - 10 = -3
				variable_partida = &heap_metadata.isFree;
				memcpy(&direccion_datos[posicion],&variable_partida[0],TAM_PAGINA);
				posicion += sizeof(heap_metadata.isFree);

				continue;
			}

			tam_pagina_restante -= sizeof(heap_metadata.size);
			heap_metadata.size = tam_pagina_restante;
			if(tam_pagina_restante >= 0){
				// ejemplo 1: 34 - 4 = 30
				memcpy(&direccion_datos[posicion],&heap_metadata.isFree,sizeof(heap_metadata.isFree));
				posicion += sizeof(heap_metadata.isFree);
			}
			else{
				// ejemplo: 7 - 10 = -3
				variable_partida = &heap_metadata.isFree;
				memcpy(&direccion_datos[posicion],&variable_partida[0],TAM_PAGINA);
				posicion += sizeof(heap_metadata.isFree);

				continue;
			}
		}
		else if(tam_pagina_restante == 0){ // es el tamano justo y no hace falta hacer nada

		}
		else{
			// ejemplo 1: 49 - 63 = -14
		}

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
