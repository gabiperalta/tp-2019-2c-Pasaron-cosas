/*
 * suse.h
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */

#ifndef SUSE_H_
#define SUSE_H_

#include <funcionesSuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <semaphore.h>
#include <hilolay2.h>

#define PATH_CONFIG = "/home/utnso/tp-2019-2c-Pasaron-cosas/suse/src/suse.config";
                       /* Estructuras*/
t_dictionary* cola_new;
t_dictionary * cola_ready;
t_dictionary* cola_exit;
t_dictionary * cola_blocked;

t_config *archivo_config;

typedef struct{
	enum estado new,ready,exec,exit;
	t_dictionary* hilos_new;
	t_dictionary* hilos_ready;
	t_dictionary* hilos_exec;
};

typedef struct{
	int cant_instancias_disponibles;
	t_list * hilos_bloqueados;
}semaforos_suse;


#endif /* SUSE_H_ */
