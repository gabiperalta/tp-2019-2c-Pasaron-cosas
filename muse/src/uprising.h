/*
 * segmentacionPaginada.h
 *
 *  Created on: 5 oct. 2019
 *      Author: utnso
 */

#ifndef UPRISING_H_
#define UPRISING_H_

#include "muse.h"

#define SEGMENTO_HEAP 0
#define SEGMENTO_MMAP 1

typedef struct{
	char* id_programa;
	t_list* tabla_segmentos;
	int socket;
}t_thread;

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

t_thread* crear_thread(char* id_programa,int socket_creado);
t_segmento* crear_segmento(uint8_t tipo);
t_pagina* crear_pagina(uint8_t bit_modificado, void* datos);

t_thread* buscar_thread(t_list* lista,int socket_thread);
t_segmento* obtener_segmento_disponible(t_list lista,uint32_t tam_solicitado);

t_desplazamiento buscar_bloque_libre(t_list* tabla_paginas,uint32_t tam);

int filtrarHeap(t_segmento* p);

#endif /* UPRISING_H_ */
