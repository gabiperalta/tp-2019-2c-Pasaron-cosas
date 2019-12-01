/*
 * hilolay_alumnos.c
 *
 *  Created on: 28 nov. 2019
 *      Author: utnso
 */


#include <hilolay/alumnos.h>
#include <hilolay/hilolay.h>
#include <hilolay/internal.h>
<<<<<<< HEAD
#include "/home/utnso/tp-2019-2c-Pasaron-cosas/biblioteca/biblioteca_sockets.h"
#include "/home/utnso/tp-2019-2c-Pasaron-cosas/biblioteca/biblioteca.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/string.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <readline/readline.h>
#include <pthread.h>
#include <sys/time.h>
#include <commons/log.h>
#include <unistd.h>

#define PATH_CONFIG "/home/utnso/tp-2019-2c-Pasaron-cosas/suse/src/suse.config"
=======
#include "suse.h"
>>>>>>> 76dff580cd951483a9e301535d84528c812a2ddc
int socket_suse;

t_config* archivo_config;
char* ip_suse;
int puerto_suse;

//ver de donde saco el IP

	// en hilolay_init abro la conexión del socket


// le paso el tid y el id del semaforo a suse
int me_wait(int tid , char * semaforo){


	t_paquete paquete_solicitud = {
				.header = SUSE_WAIT,
				.parametros = list_create()
	};

	agregar_valor(paquete_solicitud.parametros,tid);

	agregar_string(paquete_solicitud.parametros, semaforo);

	enviar_paquete(paquete_solicitud,socket_suse);

	t_paquete paquete_respuesta = recibir_paquete(socket_suse);

	int retorno = obtener_valor(paquete_respuesta.parametros); //la de funciones suse, lo que te retorna

	return retorno;
}

int me_signal(int tid, char * semaforo){


	t_paquete paquete_solicitud = {
				.header = SUSE_SIGNAL,
				.parametros = list_create()
	};

	agregar_valor(paquete_solicitud.parametros,tid);

	agregar_string(paquete_solicitud.parametros,semaforo); //agregar el semaforo al paramtro

	enviar_paquete(paquete_solicitud,socket_suse);

	t_paquete paquete_respuesta = recibir_paquete(socket_suse);

	int retorno = obtener_valor(paquete_respuesta.parametros);

	return retorno;


}

int me_join(int tid){

	t_paquete paquete_solicitud = {
				.header = SUSE_JOIN,
				.parametros = list_create()
	};

	agregar_valor(paquete_solicitud.parametros,tid);

	enviar_paquete(paquete_solicitud,socket_suse);

	t_paquete paquete_respuesta = recibir_paquete(socket_suse);

	int retorno = obtener_valor(paquete_respuesta.parametros); //la de funciones suse, lo que te retorna

	return retorno;

}

int me_create(int tid){

	t_paquete paquete_solicitud = {
				.header = SUSE_CREATE,
				.parametros = list_create()
	};

	agregar_valor(paquete_solicitud.parametros,tid);

	enviar_paquete(paquete_solicitud,socket_suse); //el que guarde al inciiar la conex)

	t_paquete paquete_respuesta = recibir_paquete(socket_suse);

	int retorno = obtener_valor(paquete_respuesta.parametros); //la de funciones suse, lo que te retorna

	return retorno;
	}
	//ponele un 0 si esta ok


		//no nmanda nada, solo necesito que suse me de el proximo hilo a executar. Quizas tengo que evaluar que tenga el programa?? Entienod que lo sabe por el socket
int me_schedule_next(){

	t_paquete paquete_solicitud = {
		.header = SUSE_SCHEDULE_NEXT,
		.parametros = NULL
	};

	enviar_paquete(paquete_solicitud,socket_suse);

	t_paquete paquete_respuesta = recibir_paquete(socket_suse);

	int retorno = obtener_valor(paquete_respuesta.parametros);

	return retorno;}


int me_close(int tid){

	t_paquete paquete_solicitud = {
			.header = SUSE_CLOSE,
			.parametros = list_create()
		};

	agregar_valor(paquete_solicitud.parametros,tid);

	enviar_paquete(paquete_solicitud,socket_suse);

	t_paquete paquete_respuesta = recibir_paquete(socket_suse);

	int retorno = obtener_valor(paquete_respuesta.parametros);

	return retorno;
}




static struct hilolay_operations funciones_suse={
	.suse_create= me_create,
	.suse_schedule_next = me_schedule_next,
	.suse_wait = me_wait,
	.suse_signal = me_signal,
	.suse_join= me_join,
	.suse_close = me_close,
};

void hilolay_init(){
	archivo_config = config_create(PATH_CONFIG);
	ip_suse =  config_get_string_value(archivo_config, "IP");
	puerto_suse = config_get_int_value(archivo_config, "LISTEN_PORT");
	socket_suse = conectarseA(ip_suse,puerto_suse);
	init_internal(&funciones_suse);

}
