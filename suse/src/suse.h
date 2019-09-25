/*
 * suse.h
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */

#ifndef SUSE_H_
#define SUSE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <semaphore.h>

                       /* Estructuras*/
t_list * cola_new;
t_list * cola_ready;
t_list* cola_exit;
t_list * cola_blocked;

typedef struct{
	int cant_instancias_disponibles;
	t_list * hilos_bloqueados;
}semaforos_suse;
                       /* Funciones */
void inicializarColaNew();
void inicializarColaReady();
void pasarDeNewAReady();
int obtenerGradoMultiprogramacion();
t_config * obtenerConfigDeSuse();
#endif /* SUSE_H_ */
