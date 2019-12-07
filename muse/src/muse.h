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
#include <sys/stat.h>
#include "../biblioteca/biblioteca_sockets.h" // en ubuntu server funciona de esta forma
#include "commons/collections/list.h"
#include "commons/bitarray.h"
#include "commons/log.h"

#include "madness.h"

#define PATH_CONFIG "/home/utnso/tp-2019-2c-Pasaron-cosas/muse/muse.config"
#define PATH_LOG "muse.log"
//#define PATH_SWAP "/home/utnso/directorio_prueba/archivo_swap.bin"
#define PATH_SWAP "archivo_swap.bin"

void* upcm;
t_bitarray* bitmap_upcm;
t_bitarray* bitmap_swap;
int cantidad_frames;
int cantidad_frames_swap;
int algoritmo_clock_frame_recorrido;
FILE* archivo_swap;

t_log* archivo_log;

int PUERTO;
int TAM_MEMORIA;
int TAM_PAGINA;
int TAM_SWAP;
uint32_t SIZE_HEAP_METADATA;

t_list* lista_procesos;
t_list* lista_clock;
t_list* lista_archivos_mmap;

pthread_mutex_t mutex_acceso_upcm;
pthread_mutex_t mutex_lista_procesos;

#endif /* MUSE_H_ */
