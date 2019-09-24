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

# define MAX_CLIENTES 50 //cantidad maxima de conexiones que pueden escucharse

int escuchar(int puerto);
int conectarseA(char *ip, int puerto);
int aceptarConexion(int socketEscucha);
//int enviarMensaje(char* mensaje, int socketDestino);
//char* recibirMensaje(int socketCliente);
void enviarSolicitud(int servidor, t_datos datos);
//void enviarResponse(int cliente,t_response response);
//void enviarCantidadDeDescribes(int cliente,uint8_t cantidadDeDescribes);
//t_request recibirRequest(int servidor);
//t_response recibirResponse(int servidor);


#endif
