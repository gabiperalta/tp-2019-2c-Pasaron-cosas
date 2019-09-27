/*
 * libmuse.c
 *
 *  Created on: 22 sep. 2019
 *      Author: utnso
 */

/*
 *
 *
 * ya fue, voy a hacer q el malloc tenga un getIdHilo, y q dependiendo
 * de q hilo sea, tengo un numero de socket
 *
 *
 *
 */

#include "libmuse.h"


int muse_init(int id, char* ip, int puerto){
	int socket_muse = conectarseA(ip,puerto);

	if(socket_muse == 0){
		return -1;
	}

	list_add(lista_conexiones,agregarConexion(id,socket_muse));
	//id_proceso = id;
	//ip_muse = malloc(strlen(ip) + 1);
	//puerto_muse = puerto;

	//int muse_servidor = conectarseA(ip,puerto);
	return 0;
}

void muse_close(){
	sleep(2);
	printf("id hilo: %d\n",(int)pthread_self());
}

uint32_t muse_alloc(uint32_t tam){
	//variable_prueba = malloc(sizeof(uint32_t));
	//variable_prueba = tam;
	//printf("id proceso: %d\n",getpid());

	printf("%d\n",list_size(lista_conexiones));

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

//================= FUNCIONES AUXILIARES =================

void __attribute__((constructor)) libmuse_init(){
	lista_conexiones = list_create();
}

t_conexion* agregarConexion(int id,int socket_creado) {
	 t_conexion* nuevo = malloc(sizeof(t_conexion));
	 nuevo->id_proceso_hilo = id;
	 nuevo->socket = socket_creado;
	 return nuevo;
}
