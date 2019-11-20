#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_

#include <stdio.h>
#include <fcntl.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <readline/readline.h>
#include "servidor.h"
#include "gestorDeMemoria.h"

// TAMANIO BLOQUE * CANTIDAD DE BLOQUES = TAMANIO MAXIMO DE DISCO
//       2^12     *        2^X          =       2^44   =>   X = 32
//		 ( 2^32 ) / 8 = 2^29  =>  ( 2^29 ) / ( 2^12 ) = 131.072	(TAMANIO DEL BITMAP EN BLOQUES)

//////// VARIABLES GLOBALES ////////

GBlock* myDisk;
int diskFD;
t_list* tablaProcesosAbiertosGlobal;
t_list* listaDeTablasDeArchivosPorProceso;
pthread_mutex_t mx_tablaGlobal;
int PUERTO;

#define PATH_CONFIG "/home/utnso/tp-2019-2c-Pasaron-cosas/sac-server/src/Config"

////////////////////////////////////


int main(int argc, char *argv[]);
size_t getFileSize(char* file);
void cargarDisco(char* diskName);

#endif /* SAC_SERVER_H_ */
