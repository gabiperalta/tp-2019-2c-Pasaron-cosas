
#include <stdbool.h>
#include <pthread.h>
#include "commons/bitarray.h"
#include "sac-server.h"

pthread_mutex_t mutexBitmap;
GBlock* myDisk;

int bloqueLibre();
int inodoLibre();
