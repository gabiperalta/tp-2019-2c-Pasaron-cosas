/*
 * funcionesSuse.c
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#include <funcionesSuse.h>


void servidor(){
	void * conectado;
	int puerto_escucha = escuchar(PUERTO);

	while((conectado=aceptarConexion(puerto_escucha))!= 1){
		// agrega procesos
		//printf("Se acepto conexion\n");
		pthread_t thread_solicitud;
		pthread_create(&thread_solicitud,NULL,(void*)procesar_solicitud,conectado);
		pthread_detach(thread_solicitud);
	}
}
void recibir_proceso(process* proceso){
	list_add(lista_procesos,proceso);
	proceso->estado = NEW;
}

void recibir_hilo(thread* hilo){
	list_add(hilos_new,hilo);
		int i = 0;
		while(!list_is_empty(hilos_new) && i<grado_multiprogramacion){
			planificarFIFO(hilo);
			i++;
	}
}

void planificarFIFO(thread* hilo){
		list_remove(hilos_new,0);
		process* proceso = obtener_proceso_asociado(hilo);
		t_list* hilosListos = proceso->hilos_ready;
		list_add(hilosListos,hilo);
		//exec
		planificarSJF(proceso);
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

void leer_config(){
	t_config* archivo_config = config_create(PATH_CONFIG);
	PUERTO = config_get_int_value(archivo_config,"LISTEN_PORT");
	grado_multiprogramacion= config_get_int_value(archivo_config,"MAX_MULTIPROG");
	tiempo_metricas = config_get_int_value(archivo_config,"METRICS_TIMER");
	alpha = config_get_int_value(archivo_config,"ALPHA_SJF");
	config_destroy(archivo_config);
}

