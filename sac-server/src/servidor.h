
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
