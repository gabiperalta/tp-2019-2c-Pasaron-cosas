/*
 * funcionesSuse.c
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */
//#ifndef FUNCIONES_SUSE_H
//#define FUNCIONES_SUSE_H
#include "funcionesSuse.h"


//primero el de largo plazo despues el de corto no en paralelo. sin hacer hilos


//tid y id del semaforo

int wait(int tid, char* id_sem, int pid){

	//uint8_t tid = hilo->tid;
	bool buscador(semaforos_suse* semaforo){
		return !strcmp(semaforo->id, id_sem);
	}
	pthread_mutex_lock(&mut_semaforos);
	semaforos_suse* semaforo = list_find(semaforos, (void*) buscador);
	pthread_mutex_unlock(&mut_semaforos);


	bool buscadorProceso(process* proceso){
		return proceso->pid == pid;
	}

	process* proceso = list_find(lista_procesos, (void*)buscadorProceso);

	thread* hilo_wait = proceso->hilo_exec;

	list_add(semaforo->hilos_bloqueados, hilo_wait); // uso las dos colas para no hacer finds
	pthread_mutex_lock(&mut_blocked);
	list_add(hilos_blocked, hilo_wait);
	pthread_mutex_unlock(&mut_blocked);//paso el thread a la cola de bloqueado
	semaforo->cant_instancias_disponibles -=1;

	log_info(suse_log,"Se bloqueo el thread");
	return 1;
}



int signal_suse(int tid, char* id_sem){


	//uint8_t tid = hilo->tid;
	bool buscador(semaforos_suse* semaforo){
			return !strcmp(semaforo->id, id_sem);
	}
	pthread_mutex_lock(&mut_semaforos);
	semaforos_suse* semaforo = list_find(semaforos, (void*) buscador);
	pthread_mutex_unlock(&mut_semaforos);
	if(semaforo->cant_instancias_disponibles <= 0){ //ver si la lista es vacia
		thread* hilo_desbloqueado = list_remove(semaforo->hilos_bloqueados,0); // por fifo
		process* proceso = obtener_proceso_asociado(hilo_desbloqueado);
		list_add(proceso->hilos_ready,hilo_desbloqueado);
		pthread_mutex_lock(&mut_blocked);

		bool condition(thread* hilo){
			return hilo->tid == tid;
		}
		list_remove_by_condition(hilos_blocked,(void*)condition);
		pthread_mutex_unlock(&mut_blocked);
		log_info(suse_log,"Desbloqueo thread");
	}

	if(semaforo->cant_instancias_disponibles < semaforo->max_valor){

		semaforo->cant_instancias_disponibles +=1;
	}
	return 1;
}

//aca tenes que planificar y devolver el prox tid a ejecutar. retornar el ID no el hilo
//Cuando se llame a esta funcion se elige el proximo tid y lo pasa estado ejecutando ademas de retornarlo

int next_tid(int pid){
	//tiene que dar proximo hilo segun el programa

	planificarCortoPlazo(pid);
	bool buscador(process* proceso){
		return proceso->pid== pid;
	}
	pthread_mutex_lock(&mut_procesos);
	process* proceso = list_find(lista_procesos, (void*)buscador);
	pthread_mutex_unlock(&mut_procesos);
	return proceso->hilo_exec->tid;

	log_info(suse_log,"Se planifica y se devuelve el next_tid");

}


int close_suse(int tid, int pid){
	sem_wait(&sem_ejecute);
	pthread_mutex_lock(&mut_procesos);
	bool buscador(process* proceso){
		return proceso->pid == pid;
	}
	process* proceso = list_find(lista_procesos, (void*)buscador);
	pthread_mutex_unlock(&mut_procesos);

	thread* hilo_ejecutando = proceso->hilo_exec;
		//ver aca memoria
		//cambiar la lista por un solo int hilo joineado
	bool buscador2(thread* hilo){
		return hilo->tid == hilo_ejecutando->tid_joineado;
	}

	if(hilo_ejecutando->tid_joineado != 0){
		pthread_mutex_lock(&mut_blocked);
		thread* hilo_joineado = list_find(hilos_blocked, (void*) buscador2);
		list_remove_by_condition(hilos_blocked, (void*) buscador2);
		pthread_mutex_unlock(&mut_blocked);
		list_add(proceso->hilos_ready, hilo_joineado);
	}
	pthread_mutex_lock(&mut_exit);
	list_add(hilos_exit,hilo_ejecutando);
	pthread_mutex_unlock(&mut_exit);

	log_info(suse_log, "Se hizo un close");

	bool condicion(thread* hilo){
		return hilo->tid == tid;
	}
	if(list_is_empty(proceso->hilos_ready) && proceso->hilo_exec == NULL){
		if(!list_any_satisfy(hilos_new, (void*) condicion) && !list_any_satisfy(hilos_blocked, (void*) condicion)){
			bool condicionProceso(process* proceso){
				return proceso->pid == pid;
			}
			pthread_mutex_lock(&mut_procesos);
			list_remove_and_destroy_by_condition(lista_procesos,(void*)condicionProceso,(void*)destructor_de_procesos);
			pthread_mutex_unlock(&mut_procesos);
			close(pid);
					//kill()
			log_info(suse_log, "Se cerro la conexión");
		}
	}
	free(hilo_ejecutando);
	return 1;
}


int crear(int tid, int pid){

	printf("Inicio crear\n");
	bool condicion(thread* hilo){
		return hilo-> pid ==  pid;
	}
	thread* hilo = malloc(sizeof(thread));

	hilo->tid= tid;
	hilo->pid= pid;
	hilo->rafagas_estimadas=0;
	hilo->tid_joineado =0;
	hilo->rafagas_ejecutadas=0;
	hilo->porcentaje_tiempo = 0;
	hilo->tiempo_ejecucion = 0;
	hilo->tiempo_ejecucion_total = 0;
	hilo->tiempo_espera = 0;
	hilo->tiempo_uso_CPU = 0;
	hilo->timestamp_final_cpu = 0;
	hilo->timestamp_inicio_cpu = 0;
	hilo->timestamp_final_ejec = 0;
	hilo->timestamp_inicio_ejec = 0;
	hilo->timestamp_inicio_espera = 0;
	hilo->timestamp_final_espera = 0;

	hilo->timestamp_inicio_ejec = getCurrentTime();

	//printf("size lista procesos %d\n",list_size(lista_procesos));
	pthread_mutex_lock(&mut_procesos);
	process* proceso = obtener_proceso_asociado(hilo);
	pthread_mutex_unlock(&mut_procesos);

	if(proceso == NULL){
		log_error(suse_log, "No se encontro proceso");
	}
	else{
	if(list_is_empty(proceso->hilos_ready) && proceso->hilo_exec == NULL){
		if(!list_any_satisfy(hilos_new, (void*) condicion) && !list_any_satisfy(hilos_blocked, (void*) condicion)){
			list_add(proceso->hilos_ready, hilo);
			log_info(suse_log, "creo hilo principal en ready");
		}
	}
	else{
		pthread_mutex_lock(&mut_new);
		list_add(hilos_new, hilo);
		pthread_mutex_unlock(&mut_new);
		}
	}
	printf("Fin crear\n");
	return 1;
}

//el tid que viene por parametro puede tener cualquier estado

int join(int tid, int pid){
	printf("llegue al join\0");
	log_info(suse_log, "inicio join");
	pthread_mutex_lock(&mut_procesos);
	bool buscador(process* proceso){
		return proceso->pid == pid;
	}
	process* proceso = list_find(lista_procesos, (void*)buscador);
	printf("proceso: %i\0", proceso->pid);
	pthread_mutex_unlock(&mut_procesos);
	bool condicion(thread* hilo){
			return hilo->tid == tid;
	}
	printf("entre\0");

	thread* hilo_prioritario = list_find(hilos_new, (void*)condicion);
	if(hilo_prioritario == NULL){
		thread* hilo_prioritario = list_find(proceso->hilos_ready, (void*)condicion);
		if(hilo_prioritario == NULL){
			thread* hilo_prioritario = list_find(hilos_blocked, (void*)condicion);
			if(hilo_prioritario == NULL){
				thread* hilo_prioritario = list_find(hilos_exit, (void*)condicion);
			}
			else{
					log_error(suse_log, "El hilo no se encuentra");
			}
		}
	}
	printf("el hilo prioritario es: %i\0", hilo_prioritario->tid);
	pthread_mutex_lock(&mut_exit);
	bool existe_en_exit = list_any_satisfy(hilos_exit, (void*)condicion);
	pthread_mutex_unlock(&mut_exit);
	if(existe_en_exit){
		 log_error(suse_log, "El hilo a ejecutar ya esta finalizado");
	}
	else{
		if(proceso->hilo_exec != NULL){
			thread* hilo_en_ejecucion= proceso->hilo_exec;
			pthread_mutex_lock(&mut_blocked);
			list_add(hilos_blocked, hilo_en_ejecucion);
			pthread_mutex_unlock(&mut_blocked);
			proceso->hilo_exec = hilo_prioritario;
			hilo_prioritario->tid_joineado = hilo_en_ejecucion->tid;
		}
		else{
			log_error(suse_log, "No hay ningun hilo ejecutando");
		}
	 }
	log_info(suse_log, "Se hizo un join");
	return 1;
}


void planificarLargoPlazo(){ // tendria que planificar cuando llega el proximo hilo
	while(1){
		int i = 0;
		while(!list_is_empty(hilos_new) && i<grado_multiprogramacion){ //VER: esto seria cuando planificar? Solo cuando pedimos next_tid, no es necesario
			sem_wait(&sem_planificacion);
			aplicarFIFO();
			i++;
		}
	}
	log_info(suse_log,"Se planifico por FIFO");
}

void planificarCortoPlazo(int pid){ //le mando el proceso por parametro??
		bool buscadorProceso(process* proceso){
			return proceso->pid == pid;//agarrar proceso
		}
		process* proceso = list_find(lista_procesos, (void*) buscadorProceso);

		t_list* hilos_listos = proceso->hilos_ready;
		while(!list_is_empty(hilos_listos) && proceso->hilo_exec != NULL){
			aplicarSJF(proceso);// sockets
		}
		sem_post(&sem_planificacion);
		log_info(suse_log, "Se planifico por SJF");
}

void aplicarFIFO(){
		pthread_mutex_lock(&mut_new);
		thread* hilo_elegido = list_remove(hilos_new,0);
		pthread_mutex_unlock(&mut_new);
		process* proceso = obtener_proceso_asociado(hilo_elegido);
		t_list* hilos_listos = proceso->hilos_ready;
		list_add(hilos_listos,hilo_elegido);
		hilo_elegido->timestamp_inicio_espera = getCurrentTime();
	}

void aplicarSJF(process* proceso) {
		t_list* aux = list_map(proceso->hilos_ready, (void*) CalcularEstimacion);
		list_sort(aux, (void*) ComparadorDeRafagas);
		thread* hilo_aux = (thread*) list_remove(aux, 0);
		bool comparator(thread* unHilo, thread* otroHilo){
			return unHilo->tid == otroHilo->tid;
		}
		int index = list_get_index(proceso->hilos_ready,hilo_aux,(void*)comparator);
		thread* hilo_a_ejecutar = list_remove(proceso->hilos_ready, index);
		hilo_a_ejecutar->timestamp_final_espera = getCurrentTime();
		uint32_t tiempoReady = (hilo_a_ejecutar->timestamp_final_espera - hilo_a_ejecutar->timestamp_inicio_espera);
		hilo_a_ejecutar->tiempo_espera += tiempoReady;

		hilo_a_ejecutar->timestamp_inicio_cpu = getCurrentTime();
		proceso->hilo_exec = hilo_a_ejecutar;
		hilo_a_ejecutar->rafagas_ejecutadas++;
		sem_post(&sem_ejecute);
		hilo_a_ejecutar->timestamp_final_cpu = getCurrentTime();
		uint32_t tiempoCPU = (hilo_a_ejecutar->timestamp_final_cpu - hilo_a_ejecutar->timestamp_inicio_cpu);
		hilo_a_ejecutar->tiempo_uso_CPU += tiempoCPU;
	}


thread* CalcularEstimacion(thread* unHilo) {
		unHilo->rafagas_estimadas = (alpha_planificacion * unHilo->rafagas_estimadas) +
		((1 - alpha_planificacion) * (unHilo->rafagas_ejecutadas));
	return unHilo;
}

bool ComparadorDeRafagas(thread* unHilo, thread* otroHilo) {
	return unHilo->rafagas_estimadas <= otroHilo->rafagas_estimadas;
}

int list_get_index(t_list* self,void* elemento,bool (*comparator)(void *,void *)){
	int longitud_de_lista = list_size(self);
	int i;
	int cont = 0;
	for(i = 0; i < longitud_de_lista; i++ ){
		if(!comparator(list_get(self,i),elemento)){
			cont++;
		}else{
			break;
		}
	}
	return cont;
}

process* obtener_proceso_asociado(thread* hilo){
	bool buscador(process* proceso){
		return proceso->pid == hilo->pid;
	}
	return list_find(lista_procesos,(void*) buscador);
}


void iniciarPlanificacion(){
		pthread_t hilo;
		pthread_create(&hilo, NULL, (void *) planificarLargoPlazo, NULL);
		log_info(suse_log, "se creo el hilo de planificacion");
		pthread_detach(hilo);
		log_info(suse_log, "Planificacion iniciada correctamente");
	}


void inicializar_listas(){
	lista_procesos = list_create();
	hilos_new = list_create();
	hilos_blocked = list_create();
	hilos_exit = list_create();
	semaforos = list_create();
}
void inicializar_semaforos(){
	pthread_mutex_init(&mut_exit,NULL);
	pthread_mutex_init(&mut_blocked,NULL);
	pthread_mutex_init(&mut_new, NULL);
	pthread_mutex_init(&mut_semaforos, NULL);
	pthread_mutex_init(&mut_procesos, NULL);
	sem_init(&sem_planificacion, 0, grado_multiprogramacion);
	sem_init(&sem_ejecute,0,1);
}

void destructor_listas(){
	list_destroy_and_destroy_elements(lista_procesos,(void*)destructor_de_procesos);
	list_destroy(hilos_new);
	list_destroy(hilos_blocked);
	list_destroy(hilos_exit);
	list_destroy_and_destroy_elements(semaforos,(void*)destructor_semaforos);
}

void destructor_semaforos(){
	sem_destroy(&sem_planificacion);
	sem_destroy(&sem_ejecute);
	pthread_mutex_destroy(&mut_blocked);
	pthread_mutex_destroy(&mut_exit);
	pthread_mutex_destroy(&mut_new);
	pthread_mutex_destroy(&mut_semaforos);
	pthread_mutex_destroy(&mut_procesos);
}
void leer_config(){
	t_config* archivo_config = config_create(PATH_CONFIG);
	grado_multiprogramacion= config_get_int_value(archivo_config,"MAX_MULTIPROG");
	tiempo_metricas = config_get_int_value(archivo_config,"METRICS_TIMER");
	alpha_planificacion = config_get_int_value(archivo_config,"ALPHA_SJF");
	ids_sem = config_get_array_value(archivo_config,"SEM_IDS");
	inicio_sem = config_get_array_value(archivo_config, "SEM_INIT");
	max_sem = config_get_array_value(archivo_config, "SEM_MAX");
	tiempo_metrics = config_get_int_value(archivo_config,"METRICS_TIMER");
	ip = config_get_string_value(archivo_config, "IP");
	puerto = config_get_int_value(archivo_config, "LISTEN_PORT");
	for(int i = 0; i< cantidadElementosCharAsteriscoAsterisco(ids_sem); i++){
		semaforos_suse* aux = malloc(sizeof(semaforos_suse));
		aux->id = malloc(strlen(ids_sem[i]) + 1);
		strcpy(aux->id, ids_sem[i]);
		aux->cant_instancias_disponibles = atoi(inicio_sem[i]);
		aux->max_valor = atoi(max_sem[i]);
		aux->hilos_bloqueados = list_create();

		pthread_mutex_lock(&mut_semaforos);

		list_add(semaforos,aux);

		pthread_mutex_unlock(&mut_semaforos);
	}
	config_destroy(archivo_config);
}

void destructor_de_procesos(process* proceso){
	log_info(suse_log,"destruyo proceso");
	list_destroy(proceso->hilos_ready);
	free(proceso->hilo_exec);
}

void destructor_de_semaforos(semaforos_suse* semaforo){
	log_info(suse_log,"destruyo semaforo");
	list_destroy(semaforo->hilos_bloqueados);
	free(semaforo->id);
}

void suse_metricas(){
	sleep(tiempo_metrics);
	process* proceso = malloc(sizeof(process));
	thread* hilo = malloc(sizeof(thread));
	hilo->porcentaje_tiempo = 0;
	hilo->tiempo_ejecucion = 0;
	hilo->tiempo_ejecucion_total = 0;
	hilo->tiempo_espera = 0;
	hilo->tiempo_uso_CPU = 0;
	hilo->timestamp_final_cpu = 0;
	hilo->timestamp_inicio_cpu = 0;
	hilo->timestamp_final_ejec = 0;
	hilo->timestamp_inicio_ejec = 0;
	hilo->timestamp_inicio_espera = 0;
	hilo->timestamp_final_espera = 0;

	uint32_t tiempoEjecucion = (hilo->timestamp_final_ejec - hilo->timestamp_inicio_ejec);
	hilo->tiempo_ejecucion += tiempoEjecucion;
	for(int i= 0; i < list_size(lista_procesos); i++){
		proceso = list_get(lista_procesos, i);
		for(int j= 0; j < list_size(proceso->hilos_ready); j++){
			hilo = list_get(proceso->hilos_ready, j);
			printf("El tiempo de espera en Ready es: %i", hilo->tiempo_espera);
			log_info(suse_log, "El tiempo de espera en Ready es: %i",hilo->tiempo_espera);
		}
		printf("El tiempo de uso de la CPU es: %i", hilo->tiempo_uso_CPU);
		log_info(suse_log, "El tiempo de uso de la CPU es: %i",hilo->tiempo_uso_CPU);
		printf("El tiempo de ejecucion es: %i", hilo->tiempo_ejecucion);
		log_info(suse_log, "El tiempo de ejecucion es: %i",hilo->tiempo_uso_CPU);
	}
	for(int i = 0; i < list_size(lista_procesos); i++){
		hilo->tiempo_ejecucion_total += hilo->tiempo_ejecucion;
	}
	if(hilo->tiempo_ejecucion_total == 0){
		hilo->porcentaje_tiempo = 0;
	}
	else{
		hilo->porcentaje_tiempo = (hilo->tiempo_ejecucion / hilo->tiempo_ejecucion_total) * 100;
	}

	printf("El porcentaje de tiempo de ejecucion es: %i", hilo->porcentaje_tiempo);
	log_info(suse_log, "El porcentaje de tiempo de ejecucion es: %i",hilo->porcentaje_tiempo);
	for(int i= 0; i < list_size(lista_procesos); i++){
			bool condition(thread* hilo){
		return hilo->pid == proceso->pid;
			}//filter proceso
		t_list* procesos_en_new = list_filter(hilos_new, (void*) condition);
		t_list* procesos_en_blocked = list_filter(hilos_blocked, (void*)condition);
		printf("La cantidad de hilos en new es: %i", list_size(procesos_en_new));
		log_info(suse_log, "La cantidad de hilos en new es: %i", list_size(procesos_en_new));
		printf("La cantidad de hilos en ready es: %i", list_size(procesos_en_blocked));
		log_info(suse_log,"La cantidad de hilos en ready es: %i", list_size(procesos_en_blocked));
		int cantidadHilosExec(){
			if(proceso->hilo_exec) return 1;
			else return 0;
		}
		printf("La cantidad de hilos en blocked es: %i", cantidadHilosExec());
		log_info(suse_log, "La cantidad de hilos en blocked es: %i", cantidadHilosExec());
		printf("El grado actual de multiprogramacion es: %i", grado_multiprogramacion);
	}

	semaforos_suse* semaforo;
	for(int i= 0; i < list_size(semaforos); i++){
		semaforo = list_get(semaforos, i);
		printf("El valor actual semaforo: %s es: %i", semaforo->id, semaforo->cant_instancias_disponibles);
		log_info(suse_log, "El valor actual semaforo: %s es: %i",semaforo->id, semaforo->cant_instancias_disponibles);
	}
}

uint32_t getCurrentTime(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

uint8_t cantidadElementosCharAsteriscoAsterisco(char** array){
	uint8_t size;
	for(size = 0; array[size] != NULL; size++);
	return size;
}

void liberarCharAsteriscoAsterisco(char** array){
	string_iterate_lines(array, (void*)free);
	free(array);
	return;
}

void iniciarMetricas(){
	pthread_create(&threadMetrics, NULL,(void*) suse_metricas, NULL);
}

void iniciarLog(){
	suse_log = log_create(PATH_LOG,"suse",false,LOG_LEVEL_INFO);
}
void destruirLog(){
	log_destroy(suse_log);
}
void destruirMetricas(){
	pthread_detach(threadMetrics);
}
//#endif
