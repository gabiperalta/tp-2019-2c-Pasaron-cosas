/*
 * biblioteca_sockets.h
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */
#ifndef BIBLIOTECA_SOCKETS_H_
#define BIBLIOTECA_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "biblioteca.h"

# define MAX_CLIENTES 100 //cantidad maxima de conexiones que pueden escucharse

int escuchar(int puerto);
int conectarseA(char *ip, int puerto);
int aceptarConexion(int socketEscucha);
//t_request recibirRequest(int servidor);
//t_response recibirResponse(int servidor);
void enviar_paquete(t_paquete paquete,int socket_servidor);
t_paquete recibir_paquete(int socket_cliente);

#endif
