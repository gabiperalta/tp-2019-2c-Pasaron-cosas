
// REVOLUCION COMPARTIDA


#include <stdio.h>
#include <stdlib.h>
#include "libmuse.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

// https://stackoverflow.com/questions/8359322/how-to-share-semaphores-between-processes-using-shared-memory
// presion_emitida,presion_recibida,revolucion_emitida,revolucion_recibida

#define NOMBRE_SEM_1 "/presion_emitida"
#define NOMBRE_SEM_2 "/presion_recibida"
#define NOMBRE_SEM_3 "/revolucion_emitida"
#define NOMBRE_SEM_4 "/revolucion_recibida"

#define RUTA_ARCHIVO "una/ruta"
#define MAP_SHARED 2

pthread_mutex_t mutex_sem_post; // controlo q al hacer post no se incremente a mas de 1

//struct hilolay_sem_t *presion_emitida;
//struct hilolay_sem_t *presion_recibida;
//struct hilolay_sem_t *revolucion_emitida;
//struct hilolay_sem_t *revolucion_recibida;
sem_t* presion_emitida;
sem_t* presion_recibida;
sem_t* revolucion_emitida;
sem_t* revolucion_recibida;

void grabar_archivo(uint32_t arch, char* palabra){
	uint32_t offset;
	sem_wait(revolucion_emitida);
	muse_get(&offset, arch, sizeof(uint32_t));
	muse_cpy(arch + offset, palabra, strlen(palabra) + 1);
	offset += strlen(palabra) + 1;
	muse_cpy(arch, &offset, sizeof(uint32_t));
	pthread_mutex_lock(&mutex_sem_post);
	int valor_actual_sem;
	sem_getvalue(revolucion_recibida,&valor_actual_sem);
	if(valor_actual_sem < 1)
		sem_post(revolucion_recibida);
	pthread_mutex_unlock(&mutex_sem_post);
	sleep(3);
}

uint32_t leer_archivo(uint32_t arch, uint32_t leido){
	uint32_t offset;
	char * palabra = malloc(100);
	sem_wait(presion_emitida);
	muse_get(&offset, arch, sizeof(uint32_t));
	uint32_t len = offset - leido;
	muse_get(palabra, arch + offset, len);
	offset += strlen(palabra) + 1;
	muse_cpy(arch, &offset, sizeof(uint32_t));
	pthread_mutex_lock(&mutex_sem_post);
	int valor_actual_sem;
	sem_getvalue(presion_recibida,&valor_actual_sem);
	if(valor_actual_sem < 1)
		sem_post(presion_recibida);
	pthread_mutex_unlock(&mutex_sem_post);
	puts(palabra);
	free(palabra);
	return offset;
}

void *revolucionar(){
	uint32_t arch = muse_map(RUTA_ARCHIVO, 4096, MAP_SHARED);
	uint32_t offset = 0;

	offset = sizeof(uint32_t);
	for(int i = 0; i<6;i++)
		offset = leer_archivo(arch, offset);

	offset = sizeof(uint32_t);
	muse_cpy(arch, &offset, sizeof(uint32_t));

	grabar_archivo(arch, "Es hora de levantarse y aprobar el TP\n");

	grabar_archivo(arch, "Es hora de aprobar y festejar\n");

	grabar_archivo(arch, "Saben que la entrega se esta por acabar\n");

	grabar_archivo(arch, "Asi que hagamoslo!\n");

	arch += 5000;

	muse_get(NULL, arch, 1);

	muse_sync(arch, 4096);

	muse_unmap(arch);

	return 0;
}

int main(void){
	/*
	sem_unlink(NOMBRE_SEM_1);
	sem_unlink(NOMBRE_SEM_2);
	sem_unlink(NOMBRE_SEM_3);
	sem_unlink(NOMBRE_SEM_4);
	return 0;
	*/
	pthread_t revolucion;

	muse_init(2, "127.0.0.1", 3306);

	//presion_emitida = hilolay_sem_open("presion_emitida");
	//presion_recibida = hilolay_sem_open("presion_recibida");
	//revolucion_emitida = hilolay_sem_open("revolucion_emitida");
	//revolucion_recibida = hilolay_sem_open("revolucion_recibida");
	presion_emitida = sem_open(NOMBRE_SEM_1,O_CREAT,S_IRWXU,0);
	presion_recibida = sem_open(NOMBRE_SEM_2,O_CREAT,S_IRWXU,1);
	revolucion_emitida = sem_open(NOMBRE_SEM_3,O_CREAT,S_IRWXU,1);
	revolucion_recibida = sem_open(NOMBRE_SEM_4,O_CREAT,S_IRWXU,0);

	//hilolay_create(&revolucion, NULL, &revolucionar, NULL);
	pthread_create(&revolucion,NULL,&revolucionar,NULL);


	//hilolay_join(&revolucion);
	pthread_join(revolucion,NULL);

	//hilolay_sem_close(presion_emitida);
	//hilolay_sem_close(presion_recibida);
	//hilolay_sem_close(revolucion_emitida);
	//hilolay_sem_close(revolucion_recibida);

	sem_close(presion_emitida);
	sem_close(presion_recibida);
	sem_close(revolucion_emitida);
	sem_close(revolucion_recibida);

	return 0;
}
