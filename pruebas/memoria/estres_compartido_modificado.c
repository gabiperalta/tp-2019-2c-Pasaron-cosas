
// ESTRES COMPARTIDO


#include <stdio.h>
#include <stdlib.h>
#include "libmuse.h"
//#include <hilolay/hilolay.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RUTA_ARCHIVO "una/ruta"
#define MAP_SHARED 2

#define NOMBRE_SEM_1 "/presion_emitida"
#define NOMBRE_SEM_2 "/presion_recibida"
#define NOMBRE_SEM_3 "/revolucion_emitida"
#define NOMBRE_SEM_4 "/revolucion_recibida"

pthread_mutex_t mutex_sem_post;

//struct hilolay_sem_t *presion_emitida;
//struct hilolay_sem_t *presion_recibida;
//struct hilolay_sem_t *revolucion_emitida;
//struct hilolay_sem_t *revolucion_recibida;
sem_t* presion_emitida;
sem_t* presion_recibida;
sem_t* revolucion_emitida;
sem_t* revolucion_recibida;

void *grabar_archivo(uint32_t arch, char* palabra)
{
	uint32_t offset;
	sem_wait(presion_recibida);
	muse_get(&offset, arch, sizeof(uint32_t));
	muse_cpy(arch + offset, palabra, strlen(palabra) + 1);

	offset += strlen(palabra) + 1;
	muse_cpy(arch, &offset, sizeof(uint32_t));
	pthread_mutex_lock(&mutex_sem_post);
	int valor_actual_sem;
	sem_getvalue(presion_emitida,&valor_actual_sem);
	if(valor_actual_sem < 1)
		sem_post(presion_emitida);
	pthread_mutex_unlock(&mutex_sem_post);

	sleep(3);
	return 0;
}

uint32_t leer_archivo(uint32_t arch, uint32_t leido)
{
	uint32_t offset;
	char * palabra = malloc(100);
	sem_wait(revolucion_recibida);
	muse_get(&offset, arch, sizeof(uint32_t));

	uint32_t len = offset - leido;
	muse_get(palabra, arch + offset, len);
	offset += strlen(palabra) + 1;

	muse_cpy(arch, &offset, sizeof(uint32_t));
	pthread_mutex_lock(&mutex_sem_post);
	int valor_actual_sem;
	sem_getvalue(revolucion_emitida,&valor_actual_sem);
	if(valor_actual_sem < 1)
		sem_post(revolucion_emitida);
	pthread_mutex_unlock(&mutex_sem_post);

	puts(palabra);
	free(palabra);
	return offset;
}

void *presionar()
{
	uint32_t arch = muse_map(RUTA_ARCHIVO, 4096, MAP_SHARED);
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

	printf("Inicia la revolucion!!!\n\n");

	offset = sizeof(uint32_t);
	for(int i = 0; i<4;i++)
		offset = leer_archivo(arch, offset);

	muse_unmap(arch);

	printf("\nAprobado\n");
	return 0;
}


int main(void)
{
	pthread_t presion;

	muse_init(getpid(), "127.0.0.1", 3306);

	presion_emitida = sem_open(NOMBRE_SEM_1,O_CREAT,S_IRWXU,0);
	presion_recibida = sem_open(NOMBRE_SEM_2,O_CREAT,S_IRWXU,1);
	revolucion_emitida = sem_open(NOMBRE_SEM_3,O_CREAT,S_IRWXU,1);
	revolucion_recibida = sem_open(NOMBRE_SEM_4,O_CREAT,S_IRWXU,0);

	pthread_create(&presion, NULL, &presionar, NULL);

	pthread_join(presion,NULL);

	sem_close(presion_emitida);
	sem_close(presion_recibida);
	sem_close(revolucion_emitida);
	sem_close(revolucion_recibida);


	return 0;
}

