/*
 * suse.h
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */

#ifndef SUSE_H_
#define SUSE_H_

#include "../../biblioteca/biblioteca_sockets.h"
#include "../../biblioteca/biblioteca.h"
#include <funcionesSuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>

#define PATH_CONFIG "/home/utnso/tp-2019-2c-Pasaron-cosas/suse/src/suse.config"

									/* Estructuras*/

int PUERTO;
int grado_multiprogramacion;
int tiempo_metricas;
int alpha;

t_list* lista_procesos;

t_config* archivo_config;

t_list* hilos_new;
t_list* hilos_blocked;
t_list* hilos_exit;

typedef struct{
	int pid; //id del proceso
	t_list* hilos_ready;
	thread* hilo_exec;
}process;


typedef struct{
	int tid; // id del hilo
	int pid; // proceso en el que esta el hilo
}thread;

typedef struct{
	int cant_instancias_disponibles;
	t_list * hilos_bloqueados;
}semaforos_suse;


#endif /* SUSE_H_ */
