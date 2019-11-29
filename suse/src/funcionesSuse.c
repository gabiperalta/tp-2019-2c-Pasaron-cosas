/*
 * funcionesSuse.c
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#include "funcionesSuse.h"


//primero el de largo plazo despues el de corto no en paralelo. sin hacer hilos

void iniciarPlanificacion(){

	log_info(suse_log,"Se inicia planificacion");

	pthread_t hilo;
	pthread_t hilo2;
	pthread_create(&hilo, NULL, (void *) planificarLargoPlazo, NULL);
	pthread_detach(hilo);
	pthread_detach(hilo2);


}

//Hay que tener en cuenta el sem max?

//ver en el signal si el numero sobrepasa el maximo y en ese caso no sumo nada

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
	log_info(suse_log,"Bloqueo thread en wait");

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

		log_info(suse_log,"desbloqueo hilo en signal");
	}

	if(cant_inst < cant max){

		semaforo->cant_instancias_disponibles +=1;
	}
}

//aca tenes que planificar y devolver el prox tid a ejecutar. retornar el ID no el hilo
//Cuando se llame a esta funcion se elige el proximo tid y lo pasa estado ejecutando ademas de retornarlo

int next_tid(int pid){
	//tiene que dar proximo hilo segun el programa

	log_info(suse_log,"Se planifica y se devuelve el next_tid");

	planificarCortoPlazo(pid);

	process* proceso = list_find(lista_procesos, (void*)buscador);

	bool buscador(process* proceso){
		return !strcmp(proceso->pid, pid);
	}

	return proceso->hilo_exec->tid;

}


void close(int tid){
	sem_wait(sem_ejecute);
	thread* hilo = list_find(lista_procesos,(void*) buscador);
	bool buscador(process* proceso){
		return !strcmp(proceso->hilo_exec->tid, tid);
	}
	process* proceso = obtener_proceso_asociado(hilo);
	thread* hilo_ejecutando = proceso->hilo_exec;

	for(int i=0; i<list_size(hilo_ejecutando->hilos_joineados); i++){
		//ver aca memoria
		//cambiar la lista por un solo int hilo joineado

		thread* hilo_joineado = list_find(hilos_blocked, (void*) buscador);

		bool buscador(thread* hilo){
			return !strcmp(hilo->tid, hilo_ejecutando->hilos_joineados[i]);
		}

		list_remove(hilos_blocked, hilo_joineado);

		list_add(proceso->hilos_ready, hilo_joineado);

	}

	list_add(hilos_exit,hilo_ejecutando);

	//if() ver si el proceso esta sin hilos asociados {

	// liberar al cliente; agarrar del tid el pid buscar si es el ultimo hilo, sacarlod e la lista de procesos hacer un close al socket y listo
	//matar el hilo del handle del proceso del socket. KILLLLL

	//}


	//eliminar tid
	//si no hay hilo se liberan las conexiones, cerrar el socket
	//en cada una de las colas?

}

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


		list_add(hilos_blocked, hilo_en_ejecucion);
		proceso->hilo_exec = hilo_prioritario;

		list_add(hilo_prioritario->hilos_joineados,hilo_en_ejecucion->tid);

	 }


}

//adentro de la estructura hilo tengo una lista de ids que fueron joineados por ese hilo. antes de cerrarlo paso todos esos hilso a ready

//close tengo que evaluar antes de cerrarlo si hay hilos que fueron joineados y liberarlos

//TODO: wait y signal, claro miras el numero si esta >0 le restas uno y si esta <=0 lo bloqueasl, lo pasas a esa cola
//signal tenes que desbloquear el hilo, dentro de cada semafoto ver que hilos bloqueo y liberas fifo. te pasa el tid del actual y tenes que desb loquear el de otro, entonces agarras el algoritmo que quieras

//join, bloquea el thread actual en le que esta (mirar el que esta ejecutando) y espera a que termine el thread que le pasas por parametro. El tid que te pasa el join es el que vas a esperar.


void planificarLargoPlazo(){ // tendria que planificar cuando llega el proximo hilo
	while(1){
		int i = 0;
		while(!list_is_empty(hilos_new) && i<grado_multiprogramacion){ //VER: esto seria cuando planificar? Solo cuando pedimos next_tid, no es necesario
			sem_wait(sem_planificacion);
			aplicarFIFO();
			i++;
		}
	}
}

void planificarCortoPlazo(int pid){ //le mando el proceso por parametro??

		//agarrar proceso

		t_list* hilos_listos = proceso->hilos_ready;
		while(!list_is_empty(hilos_listos)){
			aplicarSJFConDesalojo(proceso);// sockets
		}
		sem_post(sem_planificacion);
}

void aplicarFIFO(){
		uint64_t startTime, endTime;
		thread* hilo_elegido = list_remove(hilos_new,0);
		process* proceso = obtener_proceso_asociado(hilo_elegido);
		proceso->hilos_ready = list_create();
		t_list* hilos_listos = proceso->hilos_ready;
		startTime = getCurrentTime();
		list_add(hilos_listos,hilo_elegido);
		endTime = getCurrentTime();

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
		proceso->hilo_exec = hilo_a_ejecutar;
		hilo_a_ejecutar->rafagas_ejecutadas++;
		sem_post(sem_ejecute);


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
		list_add(semaforos,aux);
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

uint64_t getCurrentTime(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	uint64_t x = (uint64_t)( (tv.tv_sec)*1000 + (tv.tv_usec)/1000 );
	return x;
}

