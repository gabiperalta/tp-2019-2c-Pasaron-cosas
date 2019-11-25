/*
 * funcionesSuse.c
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#include <funcionesSuse.h>


//aca no tendria que venir por parametro el tid que le mandaria suse create?
//tendria que retornar un 0 o -1 dependiendo si pudo recibir bien el hilo o algo asi que sirva para el retorno de la conexión


void iniciarPlanificacion(){
	pthread_t hilo;
	pthread_create(&hilo, NULL, (void *) planificar, NULL);
	pthread_detach(hilo);
}

//tid y id del semaforo
void wait(thread* hilo, semaforos_suse* semaforo){ // cambiar por char* id y hacer list_find
	uint8_t tid = hilo->tid;
	if(semaforo->cant_instancias_disponibles >0){
		semaforo->cant_instancias_disponibles -=1;
	}
	else{
		list_add(hilos_blocked, tid);//paso el thread a la cola de bloqueado
	}

}

<<<<<<< HEAD
void signal(thread* hilo, semaforos_suse * semaforo){
	uint8_t tid = hilo->tid;
	if(semaforo->cant_instancias_disponibles >= 0){
		thread* hilo_desbloqueado = list_get(hilos_blocked,tid);
		process* proceso = obtener_proceso_asociado(hilo_desbloqueado);
		list_add(proceso->hilos_ready,hilo_desbloqueado);
	}
	else{
		semaforo->cant_instancias_disponibles +=1;
	}
=======
//aca tenes que planificar y devolver el prox tid a ejecutar. retornar el ID no el hilo
int next_tid(){



}

void signal(int tid, char * semaforo){


>>>>>>> d46abb4c1253c70eb12766a14fb95db81c17f3e5
}

void close(int tid){

	//eliminar tid
	//si no hay hilo se liberan las conexiones

}

//cuando no es hilo principal
void crear(int tid){

	//crea el hilo y lo mete en el programa que le corresponda
	//el id del programa lo tomo por el socket, quizas deberia venir por parametro algun dato de eso
}

void join(thread* hilo){
	uint8_t tid = hilo->tid;
	list_add(hilos_blocked, tid);//hay que bloquear el thread que se esta ejecutando
	//esperar a que termine el tid que envio por paramtro

}

//TODO: wait y signal, claro miras el numero si esta >0 le restas uno y si esta <=0 lo bloqueasl, lo pasas a esa cola
//signal tenes que desbloquear el hilo, dentro de cada semafoto ver que hilos bloqueo y liberas fifo. te pasa el tid del actual y tenes que desb loquear el de otro, entonces agarras el algoritmo que quieras

//close lo borra y crear hilo de usuario mete un hilo del programa

//join, bloquea el thread actual en le que esta (mirar el que esta ejecutando) y espera a que termine el thread que le pasas por parametro. El tid que te pasa el join es el que vas a esperar.

//necesito poder tener el proximo hilo a ejecutar

void planificar(){ // tendria que planificar cuando llega el proximo hilo
	while(1){
		int i = 0;
		while(!list_is_empty(hilos_new) && i<grado_multiprogramacion){
			aplicarFIFO();
			i++;
		}
	}
}

void aplicarFIFO(){
		thread* hilo_elegido = list_remove(hilos_new,0);
		process* proceso = obtener_proceso_asociado(hilo_elegido);
		t_list* hilos_listos = proceso->hilos_ready;
		list_add(hilos_listos,hilo_elegido);
		aplicarSJF(proceso);
	}

void aplicarSJF(process* proceso){
	t_list* hilos_listos = proceso->hilos_ready;
	while(!list_is_empty(hilos_listos)){
		aplicarSJFConDesalojo(proceso); // sockets
	}
	//sem_wait() del semaforo del proceso
		 // finalizar proceso
}

void aplicarSJFConDesalojo(process* proceso) {
		t_list* aux = list_map(proceso->hilos_ready, (void*) CalcularEstimacion);
		list_sort(aux, (void*) ComparadorDeRafagas);
		thread* hilo_aux = (thread*) list_remove(aux, 0);
		bool comparator(thread* unHilo, thread* otroHIlo){
			return !strcmp(unHilo->tid, otroHIlo->tid);


		int index = list_get_index(proceso->hilos_ready,hilo_aux,(void*)comparator);
		thread* hilo_a_ejecutar = list_remove(proceso->hilos_ready,index);
		proceso->hilo_exec = hilo_a_ejecutar;
		hilo_a_ejecutar->rafagas_ejecutadas++;
	}

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
	PUERTO = config_get_int_value(archivo_config,"LISTEN_PORT");
	grado_multiprogramacion= config_get_int_value(archivo_config,"MAX_MULTIPROG");
	tiempo_metricas = config_get_int_value(archivo_config,"METRICS_TIMER");
	alpha_planificacion = config_get_int_value(archivo_config,"ALPHA_SJF");
	ids_sem = config_get_array_value(archivo_config,"SEM_IDS");
	inicio_sem = config_get_array_value(archivo_config, "SEM_INIT");
	max_sem = config_get_array_value(archivo_config, "SEM_MAX");
	for(int i = 0; i< strlen(ids_sem); i++){
		semaforos_suse* aux = sizeof(semaforos_suse);
		aux->id = malloc(strlen(ids_sem[i]));
		strcpy(aux->id, ids_sem[i]);
		aux->cant_instancias_disponibles = atoi(inicio_sem[i]);
		aux->max_valor = atoi(max_sem[i]);
		list_add(semaforos,aux);
	}
	config_destroy(archivo_config);
}

void destructor_de_procesos(process* proceso){
	list_destroy(proceso->hilos_ready);
	free(proceso->hilo_exec);
}

void destructor_de_semaforos(semaforos_suse* semaforo){
	list_destroy(semaforo->hilos_bloqueados);
	free(semaforo->id);
}
