/*
 * segmentacionPaginada.h
 *
 *  Created on: 5 oct. 2019
 *      Author: utnso
 */

#ifndef SEGMENTACIONPAGINADA_H_
#define SEGMENTACIONPAGINADA_H_

#include "muse.h"

typedef struct{
	char* id_proceso_hilo;
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
	uint16_t marco;
	uint16_t bloque_swap;
	uint8_t bit_presencia;
	uint8_t bit_modificado;
}t_pagina;


t_thread* crear_thread(char* id_proceso_hilo,int socket_creado);
t_segmento* crear_segmento(uint8_t tipo);
t_pagina* crear_pagina(uint8_t bit_modificado, void* datos);

#endif /* SEGMENTACIONPAGINADA_H_ */
