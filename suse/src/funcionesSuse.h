/*
 * funcionesSuse.h
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESSUSE_H_
#define FUNCIONESSUSE_H_

#include <suse.h>



void levantarSuse();
void recibir_proceso(process* proceso);
void recibir_hilo(thread* hilo);
void planificarFIFO(process* proceso);
void planificarSJF(process* proceso);
process* obtener_proceso_asociado(thread* hilo);
thread* elegidoParaPlanificar(t_list* hilos);

int obtenerGradoMultiprogramacion();
t_config * obtenerConfigDeSuse();


#endif /* FUNCIONESSUSE_H_ */
