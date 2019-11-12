/*
 * funcionesSuse.c
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#include <funcionesSuse.h>

void procesar_solicitud(void* socket_cliente){
	t_paquete paquete = recibir_paquete(socket_cliente);
	void (*funcion_suse)(t_paquete,int);

	// por cada conexion nueva, agrego al proceso/hilo a la lista de threads

	while(paquete.error != 1){
		switch(paquete.header){
			case SUSE_CREATE:
				funcion_suse = funcion_init;
				break;
			case SUSE_SCHEDULE_NEXT:
				funcion_suse = funcion_schedule_next;
				break;
			case SUSE_WAIT:
				funcion_suse = funcion_wait;
				break;
			case SUSE_SIGNAL:
				funcion_suse = funcion_signal;
				break;
			case SUSE_JOIN:
				funcion_suse = funcion_join;
				break;
			case SUSE_CLOSE:
				funcion_suse = funcion_close;
				break
			//	return;
		}

		funcion_suse(paquete,socket_cliente);

		paquete = recibir_paquete(socket_cliente);
	}

	close(socket_cliente);
}

void funcion_init(t_paquete paquete,int socket_muse){

/*	char* id_programa = string_new();
	char* ip_socket = obtener_ip_socket(socket_muse);
	string_append(&id_programa,string_itoa(obtener_valor(paquete.parametros)));
	string_append(&id_programa,"-");
	string_append(&id_programa,ip_socket);

	list_add(lista_procesos,crear_proceso(id_programa,socket_muse));

	//				PRUEBA
	/*
	t_thread* thread_obtenido;
	for(int i=0; i<list_size(lista_threads); i++){
		thread_obtenido = list_get(lista_threads,i);
		printf("hilo nro %d\t",i);
		printf("id_programa: %s\t",thread_obtenido->id_programa);
		printf("socket: %d\t\n",thread_obtenido->socket);
	}
	printf("\n");


	free(ip_socket); // Sacar si falla
	*/

}




char* obtener_ip_socket(int s){
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	char* ipstr_reservado = malloc(sizeof(ipstr));
	//int port;

	len = sizeof addr;
	getpeername(s, (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    //port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
	    //port = ntohs(s->sin6_port);
	    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}

	strcpy(ipstr_reservado,ipstr);

	return ipstr_reservado;
}




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


void iniciarPlanificacion(){
	pthread_t hilo;
	pthread_create(&hilo, NULL, (void *) planificar, NULL);
	pthread_detach(hilo);
}

void planificar(){
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
	config_destroy(archivo_config);
}

void destructor_de_procesos(process* proceso){
	list_destroy(proceso->hilos_ready);
	free(proceso->hilo_exec);
}
