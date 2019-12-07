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

int wait(int tid, char* id_sem, int pid);
int signal_suse(int tid, char* id_sem);
int next_tid();
int close_suse(int tid, int pid);
int crear(int tid, int pid);
int join(int tid, int pid);
void planificar();
void aplicarFIFO();
void aplicarSJF(process* proceso);
void aplicarSJFConDesalojo(process* proceso);
thread* CalcularEstimacion(thread* unHilo);
bool ComparadorDeRafagas(thread* unHilo, thread* otroHilo);
int list_get_index(t_list* self,void* elemento,bool (*comparator)(void *,void *));

process* obtener_proceso_asociado(thread* hilo);

void destructor_de_procesos(process* proceso);
void destructor_de_hilos(thread* hilo);
void inicializar_listas();
void inicializar_semaforos();
void leer_config();
void planificarCortoPlazo(int pid);
void planificarLargoPlazo();
uint32_t getCurrentTime();
void destructor_semaforos();
void destructor_listas();
uint8_t cantidadElementosCharAsteriscoAsterisco(char** array);
void liberarCharAsteriscoAsterisco(char** array);
void suse_metricas();
void iniciarMetricas();
void iniciarLog();
void destruirLog();

#endif /* FUNCIONESSUSE_H_ */
