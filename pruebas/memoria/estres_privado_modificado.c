
// ESTRES PRIVADO


#include <stdio.h>
#include <stdlib.h>
#include "libmuse.h"
//#include <hilolay/hilolay.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#define NOMBRE_SEM_1 "/presion_emitida"
#define NOMBRE_SEM_2 "/presion_recibida"
#define NOMBRE_SEM_3 "/revolucion_emitida"
#define NOMBRE_SEM_4 "/revolucion_recibida"

#define RUTA_ARCHIVO "una/ruta"
#define MAP_PRIVATE 1

pthread_mutex_t mutex_sem_post;

//struct hilolay_sem_t *presion_emitida;
//struct hilolay_sem_t *presion_recibida;
sem_t* presion_emitida;
sem_t* presion_recibida;

void grabar_archivo(uint32_t arch, char* palabra)
{
	uint32_t offset;
	muse_get(&offset, arch, sizeof(uint32_t));
	muse_cpy(arch + offset, palabra, strlen(palabra) + 1);
	offset += strlen(palabra) + 1;
	printf("offset %d\n",offset);
	muse_cpy(arch, &offset, sizeof(uint32_t));
	sleep(1);
}

void *presionar()
{
	uint32_t arch = muse_map(RUTA_ARCHIVO, 4096, MAP_PRIVATE);
	uint32_t offset = 0;
	offset = sizeof(uint32_t);

	muse_cpy(arch, &offset, sizeof(uint32_t));

	grabar_archivo(arch, "Estas atrapado con la espalda contra la pared\n");

	grabar_archivo(arch, "No hay solucion ni salida\n");

	grabar_archivo(arch, "Estas luchando y nadie puede ver\n");

	grabar_archivo(arch, "La presion crece exponencialmente\n");

	grabar_archivo(arch, "Intentas mantener el paso y no podes\n");

	grabar_archivo(arch, "No aprobaras. Muajaja\n");

	muse_sync(arch, 4096);

	//hilolay_signal(presion_emitida);
	pthread_mutex_lock(&mutex_sem_post);
	int valor_actual_sem;
	sem_getvalue(presion_emitida,&valor_actual_sem);
	if(valor_actual_sem < 1){
		sem_post(presion_emitida);
		printf("WARNING: se modifico el valor de un semaforo\n");
	}
	pthread_mutex_unlock(&mutex_sem_post);

	muse_unmap(arch);
	return 0;
}

void* prueba(){
	muse_map(RUTA_ARCHIVO, 4096, MAP_PRIVATE);
	return 0;
}

int main(void){
	//sem_unlink(NOMBRE_SEM_1);
	//sem_unlink(NOMBRE_SEM_2);

	//struct hilolay_t presion;
	pthread_t presion;

	//hilolay_init();
	muse_init(getpid(), "127.0.0.1", 3306);

	//presion_emitida = hilolay_sem_open("presion_emitida");
	//presion_recibida = hilolay_sem_open("presion_recibida");
	presion_emitida = sem_open(NOMBRE_SEM_1,O_CREAT,S_IRWXU,0);
	presion_recibida = sem_open(NOMBRE_SEM_2,O_CREAT,S_IRWXU,1);

	//hilolay_create(&presion, NULL, &presionar, NULL);
	//pthread_create(&presion,NULL,&presionar,NULL);
	pthread_create(&presion,NULL,&presionar,NULL);

	//hilolay_join(&presion);
	pthread_join(presion,NULL);

	//hilolay_sem_close(presion_emitida);
	//hilolay_sem_close(presion_recibida);
	sem_close(presion_emitida);
	sem_close(presion_recibida);

	//return hilolay_return(0);

	return 0;
}
