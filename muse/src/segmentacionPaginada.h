/*
 * segmentacionPaginada.h
 *
 *  Created on: 5 oct. 2019
 *      Author: utnso
 */

#ifndef SEGMENTACIONPAGINADA_H_
#define SEGMENTACIONPAGINADA_H_

#include "muse.h"

#define SEGMENTO_HEAP 0
#define SEGMENTO_MMAP 1

#define CARGAR_DATOS 0
#define GUARDAR_DATOS 1
#define CREAR_DATOS 2

//#define SIZE_HEAP_METADATA sizeof(((t_heap_metadata*)0)->size) + sizeof(((t_heap_metadata*)0)->isFree)

typedef struct{
	char* id_programa;
	t_list* tabla_segmentos;
	int socket;
}t_proceso;

typedef struct{
	uint32_t base;
	uint32_t limite;
	t_list* tabla_paginas;
	uint8_t tipo_segmento;
}t_segmento;

typedef struct{
	//uint16_t numero_pagina;
	uint16_t frame;
	uint8_t bit_presencia;
	uint8_t bit_modificado;
}t_pagina;

typedef struct{
	uint32_t size;
	bool isFree;
}t_heap_metadata;

typedef struct{
	int numero_pagina;
	int posicion;
}t_desplazamiento;

t_proceso* crear_proceso(char* id_programa,int socket_creado);
uint32_t crear_segmento(uint8_t tipo,t_list* tabla_segmentos,uint32_t tam_solicitado);
t_pagina* crear_pagina(uint8_t bit_presencia);

t_proceso* buscar_proceso(t_list* lista,int socket_proceso);
t_segmento* buscar_segmento(t_list* tabla_segmentos,uint32_t direccion);
t_segmento* obtener_segmento_disponible(t_list lista,uint32_t tam_solicitado);
uint32_t obtener_base(t_list* tabla_segmentos);
void* obtener_datos_frame(t_pagina* pagina);
int obtener_frame_libre();

t_desplazamiento buscar_bloque_libre(t_list* tabla_paginas,uint32_t tam);

int filtrarHeap(t_segmento* p);

void cargar_datos(void* buffer,t_list* tabla_paginas,uint32_t flag_operacion,int cantidad_paginas_solicitadas);

void liberar_frame(int numero_frame);
void eliminar_pagina(t_pagina* pagina);

#endif /* SEGMENTACIONPAGINADA_H_ */
