/*
 * biblioteca_sockets.c
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */

#include "biblioteca_sockets.h"

/*	ESCUCHAR
 * Recibe un puerto y es espera conexión en ese puerto.
 * Retorna -1 en caso de que ocurra un error.
 * Retorna el socket escuchado en caso de haber recibido una conexión.
 */

int escuchar(int puerto) {
	int socketDeEscucha;
	struct sockaddr_in direccionCliente;
	direccionCliente.sin_family = AF_INET;
	direccionCliente.sin_addr.s_addr = INADDR_ANY;
	direccionCliente.sin_port = htons(puerto);

	if((socketDeEscucha = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf("Error al crear el socket de escucha\n");
		return -1;
	}
	int activador = 1;
	if(setsockopt(socketDeEscucha, SOL_SOCKET, SO_REUSEADDR, (char*) &activador, sizeof(activador)) < 0) {
		printf("Error al usar setsockopt\n");
		return -1;
	}

	if(bind(socketDeEscucha, (struct sockaddr *) &direccionCliente, sizeof(direccionCliente)) < 0) {
		printf("Error al bindear\n");
		return -1;

	}

	if(listen(socketDeEscucha, MAX_CLIENTES) < 0) {
		printf("Error al escuchar\n");
		return -1;
	}
	return socketDeEscucha;
}



/*	CONECTARSEA
 * Recibe un IP y un puerto y se conecta al SV.
 * Retonar 0 en caso de no conectarse.
 * Retonar el ID del socket cliente.
 */


int conectarseA(char* ip, int puerto) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	int cliente = socket(AF_INET, SOCK_STREAM, 0);
	if(connect(cliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {
		//perror("No se pudo conectar\n");
		return 0;
	}
	return cliente;
}

/*ACEPTARCONEXION
 * Retorna 1 en caso de no poder conectarse.
 * Retorna el ID del socket que se conectó.
 */

int aceptarConexion(int socketEscucha) {
	int nuevoSocket;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	if ((nuevoSocket = accept(socketEscucha, (struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
		return 1;
	}

	return nuevoSocket;
}

void enviarSolicitud(int servidor,t_datos datos){
	int posicion = 0;

	int tamano_buffer;
	void* buffer;

	switch(datos.header){
		case MUSE_ALLOC:

			tamano_buffer = sizeof(datos.header) + sizeof(datos.id_proceso_hilo);
			buffer = malloc(tamano_buffer);

			memcpy(&buffer[posicion],&datos.header,sizeof(datos.header));
			posicion += sizeof(datos.header);

			memcpy(&buffer[posicion],&datos.id_proceso_hilo,sizeof(datos.id_proceso_hilo));

			break;

	}

	send(servidor,buffer,tamano_buffer,0);

	free(buffer);
}

t_datos recibirRequest(int cliente){

	int bytesRecibidos;
	t_datos datos;

	void* buffer = malloc(1000); //hay que cambiar esto

	bytesRecibidos = recv(cliente, buffer, sizeof(datos.header), 0);
	datos.error = 0;

	if(bytesRecibidos <= 0) {
		perror("Se desconecto el cliente");
		datos.error = 1;
		return datos;
	}

	memcpy(&datos.header,buffer,sizeof(datos.header));

	switch(datos.header){
		case MUSE_ALLOC:

			recv(cliente, buffer, sizeof(datos.id_proceso_hilo), 0);
			memcpy(&datos.id_proceso_hilo,buffer,sizeof(datos.id_proceso_hilo));

			break;
	}

	free(buffer);

	return datos;
}
