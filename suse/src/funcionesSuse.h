/*
 * funcionesSuse.h
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESSUSE_H_
#define FUNCIONESSUSE_H_

#include <suse.h>

void servidor();
void procesar_solicitud(void* socket_cliente);
void iniciarPlanificacion();

void planificar();
void aplicarFIFO();
void aplicarSJF(process* proceso);
void aplicarSJFConDesalojo(process* proceso);
thread* CalcularEstimacion(thread* unHilo);
bool ComparadorDeRafagas(thread* unHilo, thread* otroHilo);

process* obtener_proceso_asociado(thread* hilo);
thread* elegidoParaPlanificar(t_list* hilos);
void leer_config();


#endif /* FUNCIONESSUSE_H_ */
