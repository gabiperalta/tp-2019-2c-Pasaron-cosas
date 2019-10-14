/*
 * libmuse.c
 *
 *  Created on: 22 sep. 2019
 *      Author: utnso
 */

#include "libmuse.h"


int muse_init(int id, char* ip, int puerto){

	ip_muse = string_new();
	string_append(&ip_muse,ip);
	puerto_muse = puerto;

	id_programa = id;

	return obtener_socket();
}

void muse_close(){
	close(socket_muse);
}

uint32_t muse_alloc(uint32_t tam){
	obtener_socket();

	t_paquete paquete = {
			.header = MUSE_ALLOC,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	//agregar_valor(paquete.parametros, id_programa);
	agregar_valor(paquete.parametros, tam);
	enviar_paquete(paquete,socket_muse);
	///////////////////////////////////////////////////////

	///////////////// Parametros a recibir ////////////////
	t_paquete paquete_recibido = recibir_paquete(socket_muse);
	uint32_t direccion_recibida = obtener_valor(paquete_recibido.parametros);
	printf("Direccion recibida: %d\n",direccion_recibida);
	///////////////////////////////////////////////////////

	//printf("Fin muse_alloc\n");
	return 0;
}

void muse_free(uint32_t dir){
	printf("socket del hilo: %d\n",socket_muse);
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

//void __attribute__((constructor)) libmuse_init(){
//	obtener_ip(); // para obtener esta funcion, revisar commits antes del 10/10
//}

int obtener_socket(){
	if(!conexion_realizada){
		socket_muse = conectarseA(ip_muse,puerto_muse);
		if(socket_muse == 0){ return -1;}
		conexion_realizada = true;

		t_paquete paquete = {
				.header = MUSE_INIT,
				.parametros = list_create()
		};

		///////////////// Parametros a enviar /////////////////
		agregar_valor(paquete.parametros,id_programa);
		enviar_paquete(paquete,socket_muse);
		///////////////////////////////////////////////////////

		printf("Socket obtenido correctamente\n");
	}
	return 0;
}
