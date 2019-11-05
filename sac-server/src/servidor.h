#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/config.h>


void inicializarServidor();
void atenderRequest(int socketCliente);
void crearHiloDeAtencion(int listenningSocket);


/////////  FUNCIONES SAC SERVER  /////////

funcion_init(t_paquete paquete,int socket_fuse);
funcion_getattr(t_paquete paquete,int socket_muse);
funcion_readdir(t_paquete paquete,int socket_muse);
funcion_mknod(t_paquete paquete,int socket_muse);
funcion_open(t_paquete paquete,int socket_muse);
funcion_write(t_paquete paquete,int socket_muse);
funcion_read(t_paquete paquete,int socket_muse);
funcion_unlink(t_paquete paquete,int socket_muse);
funcion_mkdir(t_paquete paquete,int socket_muse);
funcion_rmdir(t_paquete paquete,int socket_muse);
