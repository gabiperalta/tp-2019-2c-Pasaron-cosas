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

	bool buscador(semaforos_suse* semaforo){
		return !strcmp(semaforo->id, id_sem);
	}
	//pthread_mutex_lock(&mut_semaforos);
	semaforos_suse* semaforo = list_find(semaforos, (void*) buscador);
	//pthread_mutex_unlock(&mut_semaforos);


	/*bool buscadorThread(thread* hilo){
		return hilo->tid == tid;
	}*/


	process* proceso = buscarProcesoPorPid(pid);


	//pthread_mutex_lock(&proceso->mut_exec);

	//thread* hilo_wait = proceso->hilo_exec;
	
	pthread_mutex_lock(&semaforo->mutex);

	semaforo->cant_instancias_disponibles --;
	
	if(semaforo->cant_instancias_disponibles < 0){
		
		list_add(semaforo->hilos_bloqueados, proceso->hilo_exec); // uso las dos colas para no hacer finds

		pthread_mutex_lock(&mut_blocked);
		list_add(hilos_blocked, proceso->hilo_exec);//paso el thread a la cola de bloqueado
		pthread_mutex_unlock(&mut_blocked);


		proceso->hilo_exec = NULL;

		log_info(suse_log,"Se bloqueo el thread");

	}

	//pthread_mutex_unlock(&proceso->mut_exec);
	pthread_mutex_unlock(&semaforo->mutex);

	//printf("size hilos_bloqueados %d\n",list_size(semaforo->hilos_bloqueados));
	return 1;
}



int signal_suse(int tid, char* id_sem){
	//printf("Inicio signal\n");

	bool buscador(semaforos_suse* semaforo){
			return !strcmp(semaforo->id, id_sem);
	}


	semaforos_suse* semaforo = list_find(semaforos, (void*) buscador);

	pthread_mutex_lock(&semaforo->mutex);

	if(semaforo->cant_instancias_disponibles < semaforo->max_valor){

			semaforo->cant_instancias_disponibles ++;

	}

	if(semaforo->cant_instancias_disponibles <= 0){ //ver si la lista es vacia
		//printf("size hilos_bloqueados %d\n",list_size(semaforo->hilos_bloqueados));

		thread* hilo_desbloqueado = list_remove(semaforo->hilos_bloqueados,0); // por fifo

		bool buscadorHiloBloqueado(thread* hilo){
				return hilo->pid == hilo_desbloqueado->pid && hilo->tid == hilo_desbloqueado->tid;
		}

		pthread_mutex_lock(&mut_blocked);
		list_remove_by_condition(hilos_blocked, (void*)buscadorHiloBloqueado); // LO SACO DE LA LISTA GLOBAL DE BLOQUEADOS
		pthread_mutex_unlock(&mut_blocked);

		process* proceso = obtener_proceso_asociado(hilo_desbloqueado);


		pthread_mutex_lock(&proceso->mut_ready);
		list_add( proceso->hilos_ready, hilo_desbloqueado);
		sem_post(&proceso->cant_hilos_ready);
		pthread_mutex_unlock(&proceso->mut_ready);


		log_info(suse_log,"Desbloqueo thread");
	}

	pthread_mutex_unlock(&semaforo->mutex);

	//printf("Fin signal\n");
	return 1;
}

//aca tenes que planificar y devolver el prox tid a ejecutar. retornar el ID no el hilo
//Cuando se llame a esta funcion se elige el proximo tid y lo pasa estado ejecutando ademas de retornarlo

int next_tid(int pid){
	//printf("entre al next\n");


	process* proceso = buscarProcesoPorPid(pid);


	if(proceso->hilo_exec !=NULL){
		log_info(suse_log, "ya habia hilo ejecutando\n");

		actualizarTimestampEjecutado(proceso->hilo_exec);

		pthread_mutex_lock(&proceso->mut_ready);
		list_add(proceso->hilos_ready, proceso->hilo_exec);
		sem_post(&proceso->cant_hilos_ready);
		pthread_mutex_unlock(&proceso->mut_ready);

		proceso->hilo_exec = NULL;

	}
	else{
		log_info(suse_log, "se planificó porque no habia hilo ejecutando\n");
	}

	//printf("Hasta aca funciona\n");

	planificarCortoPlazo(proceso);

	//printf("proceso->hilo_exec->tid %d\n",proceso->hilo_exec->tid);

	return proceso->hilo_exec->tid;
}


int close_suse(int tid, int pid){

	int hilosEnReady;

	process* proceso = buscarProcesoPorPid(pid);


	//thread* hilo_ejecutando = proceso->hilo_exec;

	//printf("hilo_ejecutando->tid_joineado %d\n",hilo_ejecutando->tid_joineado);
	//log_info(suse_log, "hilo a cerrar %d\n",proceso->hilo_exec->tid);
	//log_info(suse_log, "tid_joineado %d\n",list_size(proceso->hilo_exec->hilos_joineado));

	int cantidadHilosJoineados = list_size(proceso->hilo_exec->hilos_joineado);

	if( cantidadHilosJoineados > 0){

		for( int i=0; i < cantidadHilosJoineados ; i++){

			thread* hiloDesbloqueado = list_remove(proceso->hilo_exec->hilos_joineado, 0);

			pthread_mutex_lock(&proceso->mut_ready);

			list_add(proceso->hilos_ready, hiloDesbloqueado);
			sem_post(&proceso->cant_hilos_ready);

			// hilosEnReady = list_size(proceso->hilos_ready);

			pthread_mutex_unlock(&proceso->mut_ready);

			//log_info(suse_log, "pongo en ready %d\n", hiloDesbloqueado->tid);

			bool buscadorHilo(thread* unHilo){
				return unHilo->pid == hiloDesbloqueado->pid && unHilo->tid == hiloDesbloqueado->tid;
			}

			pthread_mutex_lock(&mut_blocked);
			list_remove_by_condition(hilos_blocked, (void*)buscadorHilo);
			pthread_mutex_unlock(&mut_blocked);

		}

	}

	pthread_mutex_lock(&mut_exit);
	list_add(hilos_exit,proceso->hilo_exec);
	proceso->hilo_exec = NULL;
	pthread_mutex_unlock(&mut_exit);

	proceso->hilosEnSistema --;
	sem_post(&sem_multiprogramacion);

	/*if(proceso->hilo_exec->tid == tid){
		proceso->hilo_exec = NULL;
	}*/

	log_info(suse_log, "Se hizo un close");

	/*bool condicion(thread* hilo){
		return hilo->tid == tid;
	}*/
	log_info(suse_log, "Close size hilos ready %d",list_size(proceso->hilos_ready));
	log_info(suse_log, "Close size hilos new %d",list_size(hilos_new));
	log_info(suse_log, "Close size hilos blocked %d",list_size(hilos_blocked));
	log_info(suse_log, "Close size hilos exit %d",list_size(hilos_exit));

	//printf("Cantidad hilos en el sistema: %i\n", proceso->hilosEnSistema);
	//printf("Cantidad Hilos ready: %i\n", list_size(proceso->hilos_ready));
	//if(proceso->hilo_exec != NULL){
		//printf("hilo en ejecucion: %i\n", proceso->hilo_exec->tid);
	//}


	if( proceso->hilosEnSistema == 0){

		bool esElProceso(process* proceso){
			return proceso->pid == pid;
		}

		//pthread_mutex_lock(&mut_procesos);
		list_remove_and_destroy_by_condition(lista_procesos,(void*)esElProceso,(void*)destructor_de_procesos);
		//pthread_mutex_unlock(&mut_procesos);

		log_info(suse_log, "Se cerro la conexión");
	}

	return 1;
}


int crear(int tid, int pid){

	printf("Inicio crear\n");
	printf("tid %d\n",tid);


	process* proceso = buscarProcesoPorPid(pid);

	//printf("size lista procesos %d\n",list_size(lista_procesos));

	//process* proceso = obtener_proceso_asociado(hilo);


	if(proceso == NULL){
		log_error(suse_log, "No se encontro proceso");
		return -1;
	}
	else{

		thread* hilo = malloc(sizeof(thread));

		hilo->tid= tid;
		hilo->pid= pid;

		hilo->timestamp_anterior_estimado = 0;
		hilo->ultimo_timestamp_ejecutado = 0;
		hilo->ultimo_inicio_ejecucion = 0;
		hilo->hilos_joineado= list_create();

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


		proceso->hilosEnSistema ++;


		if( !proceso->tieneElHiloInicial ){
			proceso->hilo_exec = hilo;
			//hilo->timestamp_inicio_ejec = getCurrentTime();
			hilo->ultimo_inicio_ejecucion = getCurrentTime();
			proceso->tieneElHiloInicial = true;
			log_info(suse_log, "creo hilo principal en exec");
		}
		else{
			pthread_mutex_lock(&mut_new);
			list_add(hilos_new, hilo);
			//printf("size hilos new %d\n",list_size(hilos_new));
			pthread_mutex_unlock(&mut_new);
			sem_post(&sem_new);
		}
	}
	//printf("Fin crear\n");
	return 1;
}

//el tid que viene por parametro puede tener cualquier estado

int join(int tid, int pid){
	log_info(suse_log, "inicio join");

	process* proceso = buscarProcesoPorPid(pid);

	log_info(suse_log, "el hilo ejecutando es: %i \n", proceso->hilo_exec->tid);
	bool buscarEnReady(thread* hilo){
		return hilo->tid == tid;
	}
	bool buscarEnDemasListas(thread* hilo){
		return hilo->tid == tid && hilo->pid == pid;
	}


	if(proceso->hilo_exec->tid != tid){

		pthread_mutex_lock(&proceso->mut_ready);
		thread* hilo_prioritario = list_find(proceso->hilos_ready, (void*)buscarEnReady);
		pthread_mutex_unlock(&proceso->mut_ready);

		if(hilo_prioritario == NULL){

			pthread_mutex_lock(&mut_new);
			hilo_prioritario = list_find(hilos_new, (void*)buscarEnDemasListas);
			pthread_mutex_unlock(&mut_new);

			if(hilo_prioritario == NULL){

				pthread_mutex_lock(&mut_blocked);
				hilo_prioritario = list_find(hilos_blocked, (void*)buscarEnDemasListas);
				pthread_mutex_unlock(&mut_blocked);

				if(hilo_prioritario == NULL){

					pthread_mutex_lock(&mut_exit);
					hilo_prioritario = list_find(hilos_exit, (void*)buscarEnDemasListas);
					pthread_mutex_unlock(&mut_exit);

					log_error(suse_log, "El hilo a ejecutar prioritario ya esta finalizado");

					pthread_mutex_lock(&proceso->mut_ready);
					list_add(proceso->hilos_ready, proceso->hilo_exec);
					sem_post(&proceso->cant_hilos_ready);
					//printf("PROCESO DENTRO READY ANTES DE HILO IGUAL A NULL: %i	\n", ((thread*)list_get(proceso->hilos_ready, 0))->tid);
					proceso->hilo_exec = NULL;
					//printf("PROCESO DENTRO READY DESPUES DE HILO IGUAL A NULL: %i \n", ((thread*)list_get(proceso->hilos_ready, 0))->tid); // TODO VERIFICAR QUE NO SE PONGA EN NULL EN LA LISTA
					pthread_mutex_unlock(&proceso->mut_ready);

					return 1;
				}
				else{
					log_error(suse_log, "El hilo no se encuentra");
				}
			}
		}

		if(proceso->hilo_exec != NULL){

			log_info(suse_log, "se guardo el hilo en blocked %i \n", proceso->hilo_exec->tid );

			pthread_mutex_lock(&mut_blocked);
			list_add(hilos_blocked, proceso->hilo_exec);
			pthread_mutex_unlock(&mut_blocked);

			list_add(hilo_prioritario->hilos_joineado, proceso->hilo_exec);
			proceso->hilo_exec = NULL;

		}
		else{
			log_error(suse_log, "No hay ningun hilo ejecutando");
		}


	/*	return 1;

	}	thread* hilo_en_ejecucion = proceso->hilo_exec;
		pthread_mutex_lock(&mut_blocked);
		list_add(hilos_blocked, hilo_en_ejecucion);
		pthread_mutex_unlock(&mut_blocked);
		//pthread_mutex_lock(&mut_join);
		list_add(hilo_prioritario->tid_joineado,hilo_en_ejecucion->tid);
		proceso->hilo_exec = NULL;
		//pthread_mutex_unlock(&mut_join);
		//log_info(suse_log, "paso a ejecutar el tid:%i \n", proceso->hilo_exec->tid);
		//log_info(suse_log, "Prioritario: paso a ejecutar el tid:%i \n", hilo_prioritario->tid);
		//printf("hilo_prioritario->tid_joineado %d\n",hilo_prioritario->tid_joineado);
		//printf("hilo_en_ejecucion->tid %d\n",hilo_en_ejecucion->tid);

		log_info(suse_log, "se guardo el hilo en blocked %i \n", hilo_en_ejecucion->tid );

	else{
		log_error(suse_log, "No hay ningun hilo ejecutando");
	}*/

	//pthread_mutex_unlock(&mut_join);

	}
	log_info(suse_log, "Se hizo un join");

	return 1;
}

void actualizarTimestampEjecutado(thread* hilo){
	hilo->ultimo_timestamp_ejecutado = getCurrentTime() - hilo->ultimo_inicio_ejecucion;
	hilo->tiempo_ejecucion_total += hilo->ultimo_timestamp_ejecutado;

}


void planificarLargoPlazo(){ // tendria que planificar cuando llega el proximo hilo
	while(1){
		sem_wait(&sem_new);
		sem_wait(&sem_multiprogramacion);
		aplicarFIFO();
	}
}

void planificarCortoPlazo(process* proceso){

	//log_info(suse_log, "Planificar Corto Plazo: size hilos ready%d\n", list_size(proceso->hilos_ready));


	aplicarSJF(proceso);// sockets

	//printf("proceso->hilo_exec %d\n",proceso->hilo_exec->tid);

	//log_info(suse_log, "Planificar Corto Plazo: size hilos ready%d\n", list_size(proceso->hilos_ready));
	//t_list* hilos_listos = proceso->hilos_ready;
	//while(!list_is_empty(hilos_listos) && proceso->hilo_exec != NULL){
	//printf("ANTES size hilos_ready %d\n",list_size(proceso->hilos_ready));
	//printf("size hilos_new %d\n",list_size(hilos_new));
	//printf("size hilo bloqueado %d\n",list_size(hilos_blocked));
	/*if(!list_is_empty(hilos_listos)){
		aplicarSJF(proceso);// sockets
	*/

	log_info(suse_log, "Se planifico por SJF");

	//printf("size hilos_ready %d\n",list_size(proceso->hilos_ready));


	//printf("DESPUES size hilos_ready %d\n",list_size(proceso->hilos_ready));
	//if(proceso->hilo_exec != NULL){
	//	log_info(suse_log, "El hilo ejecutando es %d\n",proceso->hilo_exec->tid);
	//}
}

void aplicarFIFO(){
	//printf("Se aplica FIFO\n");
	log_info(suse_log, "Se aplica FIFO");

	pthread_mutex_lock(&mut_new);
	printf("ANTES size hilos new en FIFO %d\n",list_size(hilos_new));
	thread* hilo_elegido = list_remove(hilos_new,0);
	printf("DESPUES size hilos new en FIFO %d\n",list_size(hilos_new));
	pthread_mutex_unlock(&mut_new);


	process* proceso = obtener_proceso_asociado(hilo_elegido);


	//log_info(suse_log, "el proceso es: %i \n", proceso->pid);

	pthread_mutex_lock(&proceso->mut_ready);
	list_add(proceso->hilos_ready, hilo_elegido);
	sem_post(&proceso->cant_hilos_ready);
	log_info(suse_log, "Hay %i hilos en ready \n", list_size(proceso->hilos_ready));
	pthread_mutex_unlock(&proceso->mut_ready);

	hilo_elegido->timestamp_inicio_espera = getCurrentTime();
}

void aplicarSJF(process* proceso) {


	/*pthread_mutex_lock(&proceso->mut_ready);

	list_iterate(proceso->hilos_ready, (void*) CalcularEstimacion);
	list_sort(proceso->hilos_ready, (void*) ComparadorDeRafagas);
	thread* hilo_a_ejecutar = (thread*) list_remove(proceso->hilos_ready, 0);

	pthread_mutex_unlock(&proceso->mut_ready);
	/*t_list* aux = list_map(proceso->hilos_ready, (void*) CalcularEstimacion);
	list_sort(aux, (void*) ComparadorDeRafagas);
	thread* hilo_aux = (thread*) list_remove(aux, 0);
	bool comparator(thread* unHilo, thread* otroHilo){
		return unHilo->tid == otroHilo->tid;
	}
	int index = list_get_index(proceso->hilos_ready,hilo_aux,(void*)comparator);
	thread* hilo_a_ejecutar = list_remove(proceso->hilos_ready, index);
	printf("el hilo a ejecutar es: %i \n", hilo_a_ejecutar->tid);
	hilo_a_ejecutar->timestamp_final_espera = getCurrentTime();
	uint32_t tiempoReady = (hilo_a_ejecutar->timestamp_final_espera - hilo_a_ejecutar->timestamp_inicio_espera);
	hilo_a_ejecutar->tiempo_espera += tiempoReady;*/


	sem_wait(&proceso->cant_hilos_ready);

	pthread_mutex_lock(&proceso->mut_ready);

	list_iterate(proceso->hilos_ready, (void*) CalcularEstimacion);
	list_sort(proceso->hilos_ready, (void*) ComparadorDeRafagas);
	thread* hilo_a_ejecutar = (thread*) list_remove(proceso->hilos_ready, 0);

	pthread_mutex_unlock(&proceso->mut_ready);


	//printf("el hilo a ejecutar es: %i \n", hilo_a_ejecutar->tid);

	// PONGO A EJECUTAR AL HILO SELECCIONADO
	hilo_a_ejecutar->ultimo_inicio_ejecucion = getCurrentTime(); // lo uso para el calculo del tiempo de ejecucion
	proceso->hilo_exec = hilo_a_ejecutar;
	////////////////////////////////////////

	log_info(suse_log, "Se ejecuta el hilo: %d\n",proceso->hilo_exec->tid);

	//printf("proceso->hilo_exec->tid %d\n",proceso->hilo_exec->tid);


	// METADATA DEL HILO
	hilo_a_ejecutar->timestamp_final_espera = getCurrentTime();
	uint32_t tiempoReady = (hilo_a_ejecutar->timestamp_final_espera - hilo_a_ejecutar->timestamp_inicio_espera);
	hilo_a_ejecutar->tiempo_espera += tiempoReady;
	hilo_a_ejecutar->timestamp_inicio_cpu = getCurrentTime();
	hilo_a_ejecutar->timestamp_final_cpu = getCurrentTime();
	uint32_t tiempoCPU = (hilo_a_ejecutar->timestamp_final_cpu - hilo_a_ejecutar->timestamp_inicio_cpu);
	hilo_a_ejecutar->tiempo_uso_CPU += tiempoCPU;
}


thread* CalcularEstimacion(thread* unHilo) {
		unHilo->timestamp_anterior_estimado = (alpha_planificacion * unHilo->timestamp_anterior_estimado) +
		((1 - alpha_planificacion) * (unHilo->ultimo_timestamp_ejecutado));
	return unHilo;
}

bool ComparadorDeRafagas(thread* unHilo, thread* otroHilo) {
	return unHilo->timestamp_anterior_estimado <= otroHilo->timestamp_anterior_estimado;
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

process* buscarProcesoPorPid(uint32_t pid){
	bool buscador(process* proceso){
		return proceso->pid == pid;
	}

	//pthread_mutex_lock(&mut_procesos);
	process* proceso = list_find(lista_procesos, (void*)buscador);
	//pthread_mutex_unlock(&mut_procesos);

	return proceso;
}

process* obtener_proceso_asociado(thread* hilo){

	bool buscador(process* proceso){
		return proceso->pid == hilo->pid;
	}

	//pthread_mutex_lock(&mut_procesos);
	process* procesoEncontrado =list_find(lista_procesos,(void*) buscador);
	//pthread_mutex_unlock(&mut_procesos);

	return procesoEncontrado;

}


void iniciarPlanificacion(){
		pthread_t hilo;
		pthread_create(&hilo, NULL, (void *) planificarLargoPlazo, NULL);
		//log_info(suse_log, "se creo el hilo de planificacion");
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
	pthread_mutex_init(&mut_join,NULL);
	pthread_mutex_init(&mut_planificacion,NULL);
	sem_init(&sem_multiprogramacion, 0, grado_multiprogramacion);
	sem_init(&sem_join,0,1);
	sem_init(&sem_new,0,0);
}

void destructor_listas(){
	list_destroy_and_destroy_elements(lista_procesos,(void*)destructor_de_procesos);
	list_destroy(hilos_new);
	list_destroy(hilos_blocked);
	list_destroy_and_destroy_elements(hilos_exit, (void*)destructor_de_procesos);
	list_destroy_and_destroy_elements(semaforos,(void*)destructor_semaforos_nuestros);
}

void destructor_semaforos_nuestros(semaforos_suse* semaforo){
	free(semaforo->id);
	list_destroy(semaforo->hilos_bloqueados);
	pthread_mutex_destroy(&semaforo->mutex);
	free(semaforo);
}

void destructor_semaforos_globales(){
	sem_destroy(&sem_multiprogramacion);
	sem_destroy(&sem_join);
	sem_destroy(&sem_new);
	pthread_mutex_destroy(&mut_blocked);
	pthread_mutex_destroy(&mut_exit);
	pthread_mutex_destroy(&mut_new);
	pthread_mutex_destroy(&mut_semaforos);
	pthread_mutex_destroy(&mut_procesos);
	pthread_mutex_destroy(&mut_join);
	pthread_mutex_destroy(&mut_planificacion);
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
		semaforos_suse* semaforo = malloc(sizeof(semaforos_suse));
		semaforo->id = malloc(strlen(ids_sem[i]) + 1);
		strcpy(semaforo->id, ids_sem[i]);
		semaforo->cant_instancias_disponibles = atoi(inicio_sem[i]);
		semaforo->max_valor = atoi(max_sem[i]);
		semaforo->hilos_bloqueados = list_create();
		pthread_mutex_init(&semaforo->mutex, NULL);

		//pthread_mutex_lock(&mut_semaforos);

		list_add(semaforos,semaforo);

		//pthread_mutex_unlock(&mut_semaforos);
	}
	config_destroy(archivo_config);
}

void destructor_de_procesos(process* proceso){
	//log_info(suse_log,"destruyo proceso");
	list_destroy(proceso->hilos_ready);
	pthread_mutex_destroy(&proceso->mut_exec);
	pthread_mutex_destroy(&proceso->mut_ready);
	sem_destroy(&proceso->cant_hilos_ready);
	// free(proceso->hilo_exec); // DEBERIA SER NULL
	free(proceso);
}

void destructor_de_semaforos(semaforos_suse* semaforo){
	//log_info(suse_log,"destruyo semaforo");
	list_destroy(semaforo->hilos_bloqueados);
	pthread_mutex_destroy(&semaforo->mutex);
	free(semaforo->id);
	free(semaforo);
}

void suse_metricas(){
	while(1){
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
			log_info(suse_log, "El tiempo de espera en Ready es: %i \n",hilo->tiempo_espera);
			//printf("El tiempo de espera en Ready es: %i \n",hilo->tiempo_espera);
		}
		log_info(suse_log, "El tiempo de uso de la CPU es: %i \n",hilo->tiempo_uso_CPU);
		//printf("El tiempo de uso de la CPU es: %i \n",hilo->tiempo_uso_CPU);
		log_info(suse_log, "El tiempo de ejecucion es: %i \n",hilo->tiempo_uso_CPU);
		//printf("El tiempo de ejecucion es: %i \n",hilo->tiempo_uso_CPU);
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

	log_info(suse_log, "El porcentaje de tiempo de ejecucion es: %i \n",hilo->porcentaje_tiempo);
	//printf("El porcentaje de tiempo de ejecucion es: %i \n",hilo->porcentaje_tiempo);
	for(int i= 0; i < list_size(lista_procesos); i++){
			bool condition(thread* hilo){
		return hilo->pid == proceso->pid;
			}//filter proceso
		pthread_mutex_lock(&mut_new);
		t_list* procesos_en_new = list_filter(hilos_new, (void*) condition);
		pthread_mutex_unlock(&mut_new);
		pthread_mutex_lock(&mut_blocked);
		t_list* procesos_en_blocked = list_filter(hilos_blocked, (void*)condition);
		pthread_mutex_unlock(&mut_blocked);
		//printf("La cantidad de hilos en new es: %i \n", list_size(procesos_en_new));
		log_info(suse_log, "La cantidad de hilos en new es: %i \n", list_size(procesos_en_new));
		//printf("La cantidad de hilos en ready es: %i \n", list_size(procesos_en_blocked));
		log_info(suse_log,"La cantidad de hilos en ready es: %i \n", list_size(procesos_en_blocked));
		int cantidadHilosExec(){
			if(proceso->hilo_exec) return 1;
			else return 0;
		}
		//printf("La cantidad de hilos en blocked es: %i \n", cantidadHilosExec());
		log_info(suse_log, "La cantidad de hilos en blocked es: %i \n", cantidadHilosExec());
		//printf("El grado actual de multiprogramacion es: %i \n", grado_multiprogramacion);
	}

	semaforos_suse* semaforo;
	for(int i= 0; i < list_size(semaforos); i++){
		semaforo = list_get(semaforos, i);
		//printf("El valor actual semaforo: %s es: %i \n", semaforo->id, semaforo->cant_instancias_disponibles);
		log_info(suse_log, "El valor actual semaforo: %s es: %i \n",semaforo->id, semaforo->cant_instancias_disponibles);
	 }
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
	pthread_detach(threadMetrics);
}

void iniciarLog(){
	suse_log = log_create(PATH_LOG,"suse",false,LOG_LEVEL_INFO);
}
void destruirLog(){
	log_destroy(suse_log);
}
