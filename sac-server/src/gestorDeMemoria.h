
#include <stdbool.h>
#include <pthread.h>
#include "commons/bitarray.h"
#include "sac-server.h"

pthread_mutex_t mutexBitmap;
pthread_mutex_t mutexEscrituraInodeTable;

ptrGBloque bloqueLibre();
ptrGBloque inodoLibre();
void liberarBloqueDeDatos(ptrGBloque bloque);
ptrGBloque reservarInodo(int tipoDeArchivo);
