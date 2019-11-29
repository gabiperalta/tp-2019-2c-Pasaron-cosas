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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <readline/readline.h>
#include <pthread.h>
#include <sys/time.h>
#include <commons/log.h>

#define PATH_CONFIG "/home/utnso/tp-2019-2c-Pasaron-cosas/suse/src/suse.config"
#define PATH_LOG "/home/utnso/tp-2019-2c-Pasaron-cosas/suse/suse.log"
									/* Estructuras*/

char* ip;
int puerto;
int grado_multiprogramacion;
int tiempo_metricas;
int alpha_planificacion;
char** ids_sem;
char** inicio_sem;
char** max_sem;
int estimacion_inicial = 0;
int metrics;

t_log* suse_log;

t_list* lista_procesos;

t_list* semaforos;

t_config* archivo_config;


t_list* hilos_new;
t_list* hilos_blocked;
t_list* hilos_exit;

sem_t sem_planificacion;
sem_t sem_join;
sem_t sem_ejecute;
pthread_t threadMetrics;

typedef struct{
	uint8_t pid; //id del proceso
	t_list* hilos_ready;
	thread* hilo_exec;
}process;


typedef struct{
	uint8_t tid; // id del hilo
	uint8_t pid; // proceso en el que esta el hilo
	t_list* hilos_joineados; //lista de tid de hilos que joineo
	double rafagas_estimadas;
	int tiempo_ejecucion;
	int tiempo_espera;
	int tiempo_uso_CPU;
	int	porcentaje_tiempo;
	int rafagas_ejecutadas;
}thread;

typedef struct{
	char* id;
	int cant_instancias_disponibles;
	int max_valor;
	t_list * hilos_bloqueados;
}semaforos_suse;


#endif /* SUSE_H_ */
