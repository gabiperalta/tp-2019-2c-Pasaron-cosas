/*
 * funcionesSuse.c
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#include "funcionesSuse.h"


//primero el de largo plazo despues el de corto no en paralelo. sin hacer hilos

void iniciarPlanificacion(){

	pthread_t hilo;
	pthread_t hilo2;
	pthread_create(&hilo, NULL, (void *) planificarLargoPlazo, NULL);
	pthread_detach(hilo);
	pthread_detach(hilo2);

	log_info(suse_log, "Planificacion iniciada correctamente");

}


//tid y id del semaforo
void wait(thread* hilo, char* id_sem){

	uint8_t tid = hilo->tid;
	bool buscador(semaforos_suse* semaforo){
		return !strcmp(semaforo->id, id_sem);
	}
	semaforos_suse* semaforo = list_find(semaforos, (void*) buscador);

	list_add(semaforo->hilos_bloqueados, tid); // uso las dos colas para no hacer finds
	list_add(hilos_blocked, tid);//paso el thread a la cola de bloqueado
	semaforo->cant_instancias_disponibles -=1;

	log_info(suse_log,"Se bloqueo el thread");

}



void signal(thread* hilo, char* id_sem){


	uint8_t tid = hilo->tid;
	bool buscador(semaforos_suse* semaforo){
			return !strcmp(semaforo->id, id_sem);
	}
	semaforos_suse* semaforo = list_find(semaforos, (void*) buscador);
	if(semaforo->cant_instancias_disponibles <= 0){ //ver si la lista es vacia
		thread* hilo_desbloqueado = list_remove(semaforo->hilos_bloqueados,0); // por fifo
		process* proceso = obtener_proceso_asociado(hilo_desbloqueado);
		list_add(proceso->hilos_ready,hilo_desbloqueado);
		list_remove(hilos_blocked, tid);

		log_info(suse_log,"Desbloqueo thread");
	}

	if(semaforo->cant_instancias_disponibles < semaforo->max_valor){

		semaforo->cant_instancias_disponibles +=1;
	}
}

//aca tenes que planificar y devolver el prox tid a ejecutar. retornar el ID no el hilo
//Cuando se llame a esta funcion se elige el proximo tid y lo pasa estado ejecutando ademas de retornarlo

int next_tid(int pid){
	//tiene que dar proximo hilo segun el programa

	planificarCortoPlazo(pid);

	bool buscadorṔID(process* proceso){
				return !strcmp(proceso->pid, pid);
		}
	process* proceso = list_find(lista_procesos, (void*)buscadorṔID);


	return proceso->hilo_exec->tid;

	log_info(suse_log,"Se planifica y se devuelve el next_tid");

}


void close(int tid){
	sem_wait(sem_ejecute);
	bool buscadorClose(process* proceso){
		return !strcmp(proceso->hilo_exec->tid, tid);
	}
	thread* hilo = list_find(lista_procesos,(void*) buscadorClose);

	process* proceso = obtener_proceso_asociado(hilo);
	thread* hilo_ejecutando = proceso->hilo_exec;

	//for(int i=0; i<list_size(hilo_ejecutando->hilos_joineados); i++){
		//ver aca memoria
		//cambiar la lista por un solo int hilo joineado
	bool buscador2(thread* hilo){
		return !strcmp(hilo->tid, hilo_ejecutando->tid_joineado);
	}

	thread* hilo_joineado = list_find(hilos_blocked, (void*) buscador2);

		list_remove(hilos_blocked, hilo_joineado);

		list_add(proceso->hilos_ready, hilo_joineado);

		sem_wait(mut_exit);

		list_add(hilos_exit,hilo_ejecutando);

		sem_post(mut_exit);

		if(list_is_empty(proceso->hilo_exec) && proceso->hilo_exec == NULL){

			bool condicionProceso(process* proceso){
				return !strcmp(proceso->pid,socket_suse);
			}
			list_remove_and_destroy_by_condition(lista_procesos,(void*)condicionProceso,(void*)destructor_de_procesos);
			close(socket_suse);
			//kill()
			log_info(suse_log, "Se hizo un close");
	}
}

	//if() ver si el proceso esta sin hilos asociados {

	// liberar al cliente; agarrar del tid el pid buscar si es el ultimo hilo, sacarlod e la lista de procesos hacer un close al socket y listo
	//matar el hilo del handle del proceso del socket. KILLLLL

	//}


	//eliminar tid
	//si no hay hilo se liberan las conexiones, cerrar el socket
	//en cada una de las colas?


void crear(int tid, int program_id){

	//pthread_create
//	->tid= tid
//	->program_id= program_id
//	-> estimacion= 0
//	->rafagas=0

	//AGREGAR EL HILO A NEW
	//SI es un hilo principal PASAR A READY DIRECTOOOO!!!!! winwin

	//chequear en todas las colas si no hay ningun hilo de ese proceso para saber si es principal
}

//el tid que viene por parametro puede tener cualquier estado

void join(int tid, int pid){ // bloquea el hilo de exec hasta que termine el hilo que recibe

	process* proceso = list_find(lista_procesos, (void*)buscador);

	bool buscador(process* proceso){
		return !strcmp(proceso->pid, pid);
	}


	thread* hilo_prioritario = list_find(proceso->hilos_ready, (void*)condicion);

	bool condicion(thread* hilo){
		return !strcmp(hilo->tid, tid);
		}


	bool existe_en_exit = list_any_satisfy(hilos_exit, (void*)condicion);

	 if(existe_en_exit){
		 log_error(suse_log, "El hilo a ejecutar ya esta finalizado");
	 }

	 else{

		thread* hilo_en_ejecucion= proceso->hilo_exec;

		sem_wait(mut_blocked);

		list_add(hilos_blocked, hilo_en_ejecucion);

		sem_post(mut_blocked);

		proceso->hilo_exec = hilo_prioritario;

		hilo_prioritario->tid_joineado = hilo_en_ejecucion->tid;

	 }
	 log_info(suse_log, "Se hizo un join");

}

//adentro de la estructura hilo tengo una lista de ids que fueron joineados por ese hilo. antes de cerrarlo paso todos esos hilso a ready

//close tengo que evaluar antes de cerrarlo si hay hilos que fueron joineados y liberarlos

//join, bloquea el thread actual en le que esta (mirar el que esta ejecutando) y espera a que termine el thread que le pasas por parametro. El tid que te pasa el join es el que vas a esperar.


void planificarLargoPlazo(){ // tendria que planificar cuando llega el proximo hilo
	while(1){
		int i = 0;
		while(!list_is_empty(hilos_new) && i<grado_multiprogramacion){ //VER: esto seria cuando planificar? Solo cuando pedimos next_tid, no es necesario
			sem_wait(sem_planificacion);
			aplicarFIFO();
			i++;
		}

		log_info(suse_log,"Se planifico por FIFO");
	}
}

void planificarCortoPlazo(int pid){ //le mando el proceso por parametro??
		bool buscadorProceso(process* proceso){
			return !strcmp(proceso->pid,pid);//agarrar proceso
		}
		process* proceso = list_find(lista_procesos, (void*) buscadorProceso);

		t_list* hilos_listos = proceso->hilos_ready;
		while(!list_is_empty(hilos_listos)){
			aplicarSJFConDesalojo(proceso);// sockets
		}
		sem_post(sem_planificacion);
		log_info(suse_log, "Se planifico por SJF");
}

void aplicarFIFO(){
		thread* hilo_elegido = list_remove(hilos_new,0);
		process* proceso = obtener_proceso_asociado(hilo_elegido);
		t_list* hilos_listos = proceso->hilos_ready;
		list_add(hilos_listos,hilo_elegido);
		hilo_elegido->timestamp_inicio = clock();
	}

void aplicarSJFConDesalojo(process* proceso) {
		t_list* aux = list_map(proceso->hilos_ready, (void*) CalcularEstimacion);
		list_sort(aux, (void*) ComparadorDeRafagas);
		thread* hilo_aux = (thread*) list_remove(aux, 0);
		bool comparator(thread* unHilo, thread* otroHIlo){
			return !strcmp(unHilo->tid, otroHIlo->tid);
		}

		int index = list_get_index(proceso->hilos_ready,hilo_aux,(void*)comparator);
		thread* hilo_a_ejecutar = list_remove(proceso->hilos_ready,index);
		hilo_a_ejecutar->timestamp_final = clock();
		clock_t tiempoReady = (hilo_a_ejecutar->timestamp_final - hilo_a_ejecutar->timestamp_inicio) * 1000;
		hilo_a_ejecutar->tiempo_espera += tiempoReady;

		hilo_a_ejecutar->timestamp_inicio = clock();
		proceso->hilo_exec = hilo_a_ejecutar;
		hilo_a_ejecutar->rafagas_ejecutadas++;
		sem_post(sem_ejecute);
		hilo_a_ejecutar->timestamp_final = clock();
		clock_t tiempoCPU = (hilo_a_ejecutar->timestamp_final - hilo_a_ejecutar->timestamp_inicio) * 1000;
		hilo_a_ejecutar->tiempo_uso_CPU += tiempoCPU;

		metricasHilo(hilo_a_ejecutar);
	}


thread* CalcularEstimacion(thread* unHilo) {
	unHilo->rafagas_estimadas = (alpha_planificacion  * estimacion_inicial)
			+ ((1 - alpha_planificacion) * (unHilo->rafagas_ejecutadas));
	return unHilo;
}

bool ComparadorDeRafagas(thread* unHilo, thread* otroHilo) {
	return unHilo->rafagas_estimadas <= otroHilo->rafagas_estimadas;


process* obtener_proceso_asociado(thread* hilo){
	return list_get(lista_procesos,hilo->pid);
}

void inicializar_listas(){
	lista_procesos = list_create();
	hilos_new = list_create();
	hilos_blocked = list_create();
	hilos_exit = list_create();
	semaforos = list_create();
}
void inicializar_semaforos(){
	sem_init(mut_exit, NULL, 1);
	sem_init(mut_blocked,NULL,1);
	sem_init(mut_new, NULL, 1);
	sem_init(mut_semaforos, NULL, 1);
	sem_init(sem_planificacion,NULL, grado_multiprogramacion);
	sem_init(sem_join,NULL,1);
	sem_init(sem_ejecute,NULL,1);
}

void destructor_listas(){
	list_destroy_and_destroy_elements(lista_procesos,(void*)destructor_de_procesos);
	list_destroy(hilos_new);
	list_destroy(hilos_blocked);
	list_destroy(hilos_exit);
	list_destroy_and_destroy_elements(semaforos,(void*)destructor_de_semaforos);
}

void destructor_semaforos(){
	sem_destroy(sem_planificacion);
	sem_destroy(sem_join);
	sem_destroy(sem_ejecute);
	sem_destroy(mut_blocked);
	sem_destroy(mut_exit);
	sem_destroy(mut_new);
	sem_destroy(mut_semaforos);
}
void leer_config(){
	t_config* archivo_config = config_create(PATH_CONFIG);
	grado_multiprogramacion= config_get_int_value(archivo_config,"MAX_MULTIPROG");
	tiempo_metricas = config_get_int_value(archivo_config,"METRICS_TIMER");
	alpha_planificacion = config_get_int_value(archivo_config,"ALPHA_SJF");
	ids_sem = config_get_array_value(archivo_config,"SEM_IDS");
	inicio_sem = config_get_array_value(archivo_config, "SEM_INIT");
	max_sem = config_get_array_value(archivo_config, "SEM_MAX");
	metrics = config_get_int_value(archivo_config,"METRICS_TIMER");
	ip = config_get_string_value(archivo_config, "IP");
	puerto = config_get_int_value(archivo_config, "LISTEN_PORT");
	for(int i = 0; i< strlen(ids_sem); i++){
		semaforos_suse* aux = sizeof(semaforos_suse);
		aux->id = malloc(strlen(ids_sem[i]));
		strcpy(aux->id, ids_sem[i]);
		aux->cant_instancias_disponibles = atoi(inicio_sem[i]);
		aux->max_valor = atoi(max_sem[i]);
		aux->hilos_bloqueados = list_create();

		sem_wait(mut_semaforos);

		list_add(semaforos,aux);

		sem_post(mut_semaforos);
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

void metricasHilo(thread* hilo){
	process* proceso = obtener_proceso_asociado(hilo);
	for(int i = 0; i<list_size(proceso->hilos_ready); i++){
		printf("El tiempo de espera en Ready es: %i", hilo->tiempo_espera);
	}
	printf("El tiempo de uso de la CPU es: %i", hilo->tiempo_uso_CPU);
}

void metricasPrograma(process proceso){
	printf("El grado actual de multiprogramacion es: %i", grado_multiprogramacion);
	printf("La cantidad de hilos en READY es: %i", list_size(hilos_new)); //por cada programa
	printf("La cantidad de hilos en READY es: %i", list_size(proceso->hilos_ready));
	printf("La cantidad de hilos en BLOCKED es: %i",list_size(hilos_blocked));

}

void metricasSemaforo(semaforos_suse* semaforo){
	for(int i= 0; i < list_size(semaforos); i++){
		printf("El Valor actual semaforo: %s es: %i", semaforo->id, semaforo->cant_instancias_disponibles);
	}
}

void metricas(){
	sleep(metrics);

}
