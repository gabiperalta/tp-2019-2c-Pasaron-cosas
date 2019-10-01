/*
 * libmuse.c
 *
 *  Created on: 22 sep. 2019
 *      Author: utnso
 */

#include "libmuse.h"


int muse_init(int id, char* ip, int puerto){

	if(ip_programa == NULL){
		obtener_ip();		// esto no funciona si la VM no tiene conexion
		printf("ip del programa actual: %s\n",ip_programa);
	}

	socket_muse = conectarseA(ip,puerto);

	if(socket_muse == 0){
		return -1;
	}

	id_proceso_hilo = string_new();
	string_append(&id_proceso_hilo,string_itoa(id));
	string_append(&id_proceso_hilo,"-");
	string_append(&id_proceso_hilo,ip_programa);

	t_paquete paquete = {
			.header = MUSE_INIT,
			.parametros = list_create()
	};
	//agregar_valor(paquete.parametros,25);
	agregar_string(paquete.parametros,id_proceso_hilo);

	enviar_paquete(paquete,socket_muse);

	//close(socket_muse); // deberia estar en muse_close

	return 0;
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
	//variable_prueba = malloc(sizeof(uint32_t));
	//variable_prueba = tam;
	//printf("id proceso: %d\n",getpid());

	//printf("%d\n",list_size(lista_conexiones));

	//extern int variable_prueba;
	//variable_prueba = (int) tam;


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

//void __attribute__((constructor)) libmuse_init(){
//	obtener_ip();
//}

void obtener_ip(){
	int fd;
	struct ifreq ifr;
	//int tam_ip;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to "eth0" */
	strncpy(ifr.ifr_name, "enp0s3", IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	//tam_ip = strlen(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr)) + 1;
	//ip_programa = malloc(tam_ip);
	//strcpy()

	/* display result */
	//printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	ip_programa = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}
