#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/config.h>
#include "biblioteca_sockets.h"
#include "funcionesSac.h"


void inicializarServidor();
void servidor();
void procesar_solicitud(void *socket_cliente);


/////////  FUNCIONES SAC SERVER  /////////

void funcion_init(t_paquete paquete,int socket_fuse);
void funcion_getattr(t_paquete paquete,int socket_muse);
void funcion_readdir(t_paquete paquete,int socket_muse);
void funcion_mknod(t_paquete paquete,int socket_muse);
void funcion_open(t_paquete paquete,int socket_muse);
void funcion_write(t_paquete paquete,int socket_muse);
void funcion_read(t_paquete paquete,int socket_muse);
void funcion_unlink(t_paquete paquete,int socket_muse);
void funcion_mkdir(t_paquete paquete,int socket_muse);
void funcion_rmdir(t_paquete paquete,int socket_muse);

#endif /* SERVIDOR_H_ */
