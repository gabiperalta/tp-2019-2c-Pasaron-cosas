/*
 * funcionesSuse.c
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#include <funcionesSuse.h>

void levantarSuse(){
	levantarServidor();
	//char* elemento = recibir_elemento();
	//if(elemento == "proceso"){
		//recibir_proceso(elemento);
	//}
	//else if (elemento == "hilo"){
		//recibir_hilo(elemento);
	//}
}

void servidor(){
	void * conectado;
	int puerto_escucha = escuchar(PUERTO);

	while((conectado=aceptarConexion(puerto_escucha))!= 1){

		//printf("Se acepto conexion\n");
		pthread_t thread_solicitud;
		pthread_create(&thread_solicitud,NULL,(void*)procesar_solicitud,conectado);
		pthread_detach(thread_solicitud);
	}
}
void recibir_proceso(process* proceso){
	queue_push(q_procesos,proceso);
	dictionary_put(d_procesos,string_itoa(proceso->pid),proceso);
	proceso->estado = NEW;
}

void recibir_hilo(thread* hilo){
	process* proceso = obtener_proceso_asociado(hilo);
	if(proceso->estado == NEW){
		queue_push(q_procesos,hilo);
		dictionary_put(d_procesos,string_itoa(hilo->pid),hilo);
	}
	else{
		dictionary_put(proceso->hilos_ready,string_itoa(hilo->pid),hilo);
	}
	planificarFIFO(proceso); //Planificacion FIFO
}

void planificarFIFO(process* proceso){
	int i= 0;
	t_list* hilosListos = proceso->hilos_ready;
	int grado_multiprog = obtenerGradoMultiprogramacion();
	while(i<grado_multiprog && !list_is_empty(hilosListos)){
		int pid = queue_peek(q_procesos);
		int p = dictionary_get(d_procesos,string_itoa(pid));
		if(list_size(hilosListos) > 0){
			list_remove(hilosListos,i);
		}
		else{
			i++;
		}
		//exec
		planificarSJF(p);
	}
}

void planificarSJF(process* proceso){
	t_list* hilosListos = proceso->hilos_ready;
	while(!list_is_empty(hilosListos)){
		thread* hilo = elegidoParaPlanificar(hilosListos);
		proceso->hilo_exec = hilo;
		avisarAHilolay(hilo); // sockets
	}
	//sem_wait() del semaforo del proceso
		 // finalizar proceso
}
thread* elegidoParaPlanificar(t_list* hilos){
	while(1){
		clock()
	}
	return
}
process* obtener_proceso_asociado(thread* hilo){
	return dictionary_get(d_procesos,string_itoa(hilo->pid));
}

int obtenerGradoMultiprogramacion(){
	t_config * config = obtenerConfigDeSuse();
	int grado_multiprog = config_get_int_value(config,"MAX_MULTIPROG");
	return grado_multiprog;
}

t_config * obtenerConfigDeSuse(){
	t_config * config = config_create(PATH_CONFIG);
	return config;
}
