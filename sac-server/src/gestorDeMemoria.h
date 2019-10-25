
#include <stdbool.h>
#include <pthread.h>
#include "commons/bitarray.h"
#include "sac-server.h"

pthread_mutex_t mutexBitmap;
pthread_mutex_t mutexEscrituraInodeTable;
GBlock* myDisk;

int bloqueLibre();
int inodoLibre();
void liberarBloque(int bloque);
int reservarInodo(int tipoDeArchivo);
