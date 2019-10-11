/*
 * libmuse.c
 *
 *  Created on: 22 sep. 2019
 *      Author: utnso
 */

#include "libmuse.h"


int muse_init(int id, char* ip, int puerto){
	//if(ip_programa == NULL){
	//	obtener_ip();		// esto no funciona si la VM no tiene conexion
	//	printf("ip del programa actual: %s\n",ip_programa);
	//}
	// Ahora obtengo la IP desde MUSE

	ip_muse = string_new();
	string_append(&ip_muse,ip);
	puerto_muse = puerto;

	id_programa = id;

	//obtener_socket();

	//socket_muse = conectarseA(ip,puerto);
	//if(socket_muse == 0){
	//	return -1;
	//}
	//conexion_realizada = true;

	//t_paquete paquete = {
	//		.header = MUSE_INIT,
	//		.parametros = list_create()
	//};

	///////////////// Parametros a enviar /////////////////
	//agregar_valor(paquete.parametros,id_programa);
	//enviar_paquete(paquete,socket_muse);
	///////////////////////////////////////////////////////


	///////////////// Parametros a recibir ////////////////
	//t_paquete paquete_recibido = recibir_paquete(socket_muse);
	///////////////////////////////////////////////////////

	printf("Fin muse_init\n");
	return obtener_socket();
}

void muse_close(){
	//sleep(2);
	//printf("id hilo: %d\n",(int)pthread_self());
	//printf("variable de prueba: %d\n",variable_prueba);
	//printf("%s\n",id_proceso_hilo);
	//printf("%d\n",strlen(id_proceso_hilo));
	close(socket_muse);
}

uint32_t muse_alloc(uint32_t tam){
	obtener_socket();
	//printf("id proceso: %d\n",getpid());

	//printf("%d\n",list_size(lista_conexiones));

	//extern int variable_prueba;
	//variable_prueba = (int) tam;

	/*
	t_paquete paquete = {
			.header = MUSE_ALLOC,
			.parametros = list_create()
	};

	///////////////// Parametros a enviar /////////////////
	//agregar_valor(paquete.parametros, id_programa);
	agregar_valor(paquete.parametros, tam);
	///////////////////////////////////////////////////////
	enviar_paquete(paquete,socket_muse);

	///////////////// Parametros a recibir ////////////////
	t_paquete paquete_recibido = recibir_paquete(socket_muse);
	printf("valor recibido: %d\n",obtener_valor(paquete_recibido.parametros));
	///////////////////////////////////////////////////////
	*/

	printf("Fin muse_alloc\n");
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
