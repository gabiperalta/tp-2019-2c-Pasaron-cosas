/*
 * funcionesSuse.h
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESSUSE_H_
#define FUNCIONESSUSE_H_

#include <suse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <semaphore.h>
#include <pthread.h>

void inicializarColaNew();
void inicializarColaReady();
void pasarDeNewAReady();
int obtenerGradoMultiprogramacion();
t_config * obtenerConfigDeSuse();


#endif /* FUNCIONESSUSE_H_ */
