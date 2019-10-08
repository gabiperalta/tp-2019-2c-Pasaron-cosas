/*
 * funciones.c
 *
 *  Created on: 3 oct. 2019
 *      Author: utnso
 */

#include "funcionesMuse.h"

void procesar_solicitud(void* socket_cliente){
	t_paquete paquete = recibir_paquete(socket_cliente);
	void (*funcion_muse)(t_paquete,int);

	while(paquete.error != 1){
		switch(paquete.header){
			case MUSE_INIT:
				funcion_muse = funcion_init;
				break;
			case MUSE_ALLOC:
				funcion_muse = funcion_alloc;
				break;
			case MUSE_CLOSE:
				return;
		}

		funcion_muse(paquete,socket_cliente);

		paquete = recibir_paquete(socket_cliente);
	}

	close(socket_cliente);

}

void leer_config(){
	t_config* archivo_config = config_create(PATH_CONFIG);
	PUERTO = config_get_int_value(archivo_config,"LISTEN_PORT");
	TAM_MEMORIA = config_get_int_value(archivo_config,"MEMORY_SIZE");
	TAM_PAGINA = config_get_int_value(archivo_config,"PAGE_SIZE");
	TAM_SWAP = config_get_int_value(archivo_config,"SWAP_SIZE");
	config_destroy(archivo_config);
}

void init_threads(){
	pthread_t thread_servidor;
	pthread_create(&thread_servidor,NULL,(void*)servidor,NULL);
	pthread_detach(thread_servidor);
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

void funcion_init(t_paquete paquete,int socket_muse){
	list_add(lista_threads,crear_thread(obtener_string(paquete.parametros),socket_muse));
}

void funcion_alloc(t_paquete paquete,int socket_muse){
	int tam = obtener_valor(paquete.parametros);
	t_thread* thread_encontrado = buscar_thread(lista_threads,socket_muse);
	t_thread* thread_nuevo;

	if(thread_encontrado == NULL){
		printf("No se inicializo libmuse\n");
		return;
	}


}
