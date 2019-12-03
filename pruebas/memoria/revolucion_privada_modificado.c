#include <stdio.h>
#include <stdlib.h>
#include "../tp-2019-2c-Pasaron-cosas/LibMuse/libmuse.h"
#include <hilolay/hilolay.h>
#include <string.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NOMBRE_SEM_1 "/presion_emitida"
#define NOMBRE_SEM_2 "/presion_recibida"
#define NOMBRE_SEM_3 "/revolucion_emitida"
#define NOMBRE_SEM_4 "/revolucion_recibida"

#define RUTA_ARCHIVO "una/ruta"
#define MAP_PRIVATE 1

pthread_mutex_t mutex_sem_post;

//struct hilolay_sem_t* presion_emitida;
//struct hilolay_sem_t* presion_recibida;
//struct hilolay_sem_t* revolucion_emitida;
//struct hilolay_sem_t* revolucion_recibida;
sem_t* presion_emitida;
sem_t* presion_recibida;
sem_t* revolucion_emitida;
sem_t* revolucion_recibida;

uint32_t leer_archivo(uint32_t arch, uint32_t leido)
{
	uint32_t offset;
	char * palabra = malloc(100);
	//hilolay_wait(revolucion_emitida);
	sem_wait(revolucion_emitida);
	muse_get(&offset, arch, sizeof(uint32_t));
	uint32_t len = offset - leido;
	muse_get(palabra, arch + offset, len);
	offset += strlen(palabra) + 1;
	muse_cpy(arch, &offset, sizeof(uint32_t));
	//hilolay_signal(revolucion_recibida);
	pthread_mutex_lock(&mutex_sem_post);
	int valor_actual_sem;
	sem_getvalue(revolucion_recibida,&valor_actual_sem);
	if(valor_actual_sem < 1)
		sem_post(revolucion_recibida);
	pthread_mutex_unlock(&mutex_sem_post);
	puts(palabra);
	free(palabra);
	return offset;
}

void *revolucionar()
{
	//hilolay_wait(presion_emitida);
	sem_wait(presion_emitida);
	uint32_t arch = muse_map(RUTA_ARCHIVO, 4096, MAP_PRIVATE);
	uint32_t offset = 0;

	offset = sizeof(uint32_t);
	for(int i = 0; i<6;i++)
		offset = leer_archivo(arch, offset);

	arch += 5000;

	muse_get(NULL, arch, 1);

	muse_sync(arch, 4096);

	muse_unmap(arch);
	return 0;
}


int main(void)
{
	//struct hilolay_t revolucion;
	pthread_t revolucion;

	//hilolay_init();
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
	pthread_join(revolucionar,NULL);

	//hilolay_sem_close(presion_emitida);
	//hilolay_sem_close(presion_recibida);
	//hilolay_sem_close(revolucion_emitida);
	//hilolay_sem_close(revolucion_recibida);
	sem_close(presion_emitida);
	sem_close(presion_recibida);
	sem_close(revolucion_emitida);
	sem_close(revolucion_recibida);

	//return hilolay_return(0);
	return 0;
}
