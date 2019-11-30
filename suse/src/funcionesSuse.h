/*
 * funcionesSuse.h
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESSUSE_H_
#define FUNCIONESSUSE_H_

#include "suse.h"

void iniciarPlanificacion();

void wait(int tid, char* id_sem);
void signal_suse(int tid, char* id_sem);
int next_tid();
void close(int tid);
void crear(int tid);
void join(int tid);
void planificar();
void aplicarFIFO();
void aplicarSJF(process* proceso);
void aplicarSJFConDesalojo(process* proceso);
thread* CalcularEstimacion(thread* unHilo);
bool ComparadorDeRafagas(thread* unHilo, thread* otroHilo);

process* obtener_proceso_asociado(thread* hilo);

void destructor_de_procesos(process* proceso);
void destructor_de_hilos(thread* hilo);
void inicializar_listas();
void inicializar_semaforos();
void destructor_listas();
void destructor_semaforos();
void leer_config();
void metricas();
void planificarCortoPlazo(int pid);
void planificarLargoPlazo();


#endif /* FUNCIONESSUSE_H_ */
