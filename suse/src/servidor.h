/*
 * servidor.h
 *
 *  Created on: 1 dic. 2019
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "suse.h"
#include "biblioteca.h"
#include "biblioteca_sockets.h"


void inicializarServidor();
void servidor();
void procesar_solicitud(void *socket_cliente);
char* obtener_ip_socket(int s);

/////////  FUNCIONES SUSE  /////////

void funcion_create(t_paquete paquete,int socket_suse);
void funcion_schedule_next(t_paquete paquete,int socket_suse);
void funcion_wait(t_paquete paquete,int socket_suse);
void funcion_signal(t_paquete paquete,int socket_suse);
void funcion_join(t_paquete paquete,int socket_suse);
void funcion_close(t_paquete paquete,int socket_suse);

#endif /* SERVIDOR_H_ */
