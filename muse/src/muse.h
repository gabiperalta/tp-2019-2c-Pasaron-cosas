/*
 * muse.h
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#ifndef MUSE_H_
#define MUSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <pthread.h>
#include "biblioteca_sockets.h"
#include "commons/collections/list.h"
#include "commons/bitarray.h"

#include "funcionesMuse.h"

#define IP "127.0.0.1"
//#define PUERTO 34043
#define PATH_CONFIG "/home/utnso/tp-2019-2c-Pasaron-cosas/muse/muse.config"
#define PATH_SWAP "archivo_swap.bin"

void* upcm;
t_bitarray* bitmap_upcm;
t_bitarray* bitmap_swap;
int cantidad_frames;
int cantidad_frames_swap;
int algoritmo_clock_frame_recorrido;
FILE* archivo_swap;

int PUERTO;
int TAM_MEMORIA;
int TAM_PAGINA;
int TAM_SWAP;
uint32_t SIZE_HEAP_METADATA;

t_list* lista_procesos;
t_list* lista_clock;

pthread_mutex_t mutex_acceso_upcm;
pthread_mutex_t mutex_lista_procesos;

#endif /* MUSE_H_ */
