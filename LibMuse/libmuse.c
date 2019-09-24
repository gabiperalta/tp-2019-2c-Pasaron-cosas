/*
 * libmuse.c
 *
 *  Created on: 22 sep. 2019
 *      Author: utnso
 */



#include "libmuse.h"

int muse_init(int id, char* ip, int puerto){

	//id_proceso = id;
	//ip_muse = malloc(strlen(ip) + 1);
	//puerto_muse = puerto;

	//int muse_servidor = conectarseA(ip,puerto);
	return 0;
}

void muse_close(){
	sleep(2);
	printf("id proceso: %d\n",getpid());
}

uint32_t muse_alloc(uint32_t tam){

	printf("id proceso: %d\n",getpid());

	return 0;
}

void muse_free(uint32_t dir){

}

int muse_get(void* dst, uint32_t src, size_t n){
	return 0;
}

int muse_cpy(uint32_t dst, void* src, int n){
	return 0;
}

uint32_t muse_map(char *path, size_t length, int flags){
	return 0;
}

int muse_sync(uint32_t addr, size_t len){
	return 0;
}

int muse_unmap(uint32_t dir){
	return 0;
}
