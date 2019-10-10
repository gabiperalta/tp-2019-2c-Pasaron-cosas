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

/*
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
*/

// Si no se envia la lista de parametros, inicializarla con NULL
void enviar_paquete(t_paquete paquete,int socket_servidor){
	void* buffer;
	int posicion = 0;
	uint8_t cantidad_parametros = 0;
	int tam_buffer = sizeof(paquete.header) + sizeof(tam_buffer) + sizeof(cantidad_parametros);
	t_parametro* parametro;

	if(paquete.parametros != NULL){
		cantidad_parametros = list_size(paquete.parametros);

		for(int i=0;i<cantidad_parametros;i++){
			parametro = list_get(paquete.parametros,i);
			tam_buffer += sizeof(parametro->valor) + sizeof(parametro->recibir_string);
			if(parametro->recibir_string)
				tam_buffer += parametro->valor;
		}
	}

	buffer = malloc(tam_buffer);

	memcpy(&buffer[posicion],&tam_buffer,sizeof(tam_buffer)); //mando el tamano del buffer para saber el total a recibir
	posicion += sizeof(tam_buffer);
	memcpy(&buffer[posicion],&paquete.header,sizeof(paquete.header));
	posicion += sizeof(paquete.header);
	memcpy(&buffer[posicion],&cantidad_parametros,sizeof(cantidad_parametros));
	posicion += sizeof(cantidad_parametros);

	if(paquete.parametros != NULL){
		for(int i=0;i<cantidad_parametros;i++){
			parametro = list_get(paquete.parametros,i);

			memcpy(&buffer[posicion],&parametro->valor,sizeof(parametro->valor));
			posicion += sizeof(parametro->valor);

			memcpy(&buffer[posicion],&parametro->recibir_string,sizeof(parametro->recibir_string));
			posicion += sizeof(parametro->recibir_string);

			if(!parametro->recibir_string){ continue; }
			memcpy(&buffer[posicion],parametro->valor_string,parametro->valor);
			posicion += parametro->valor;
		}
	}
	//printf("tamano buffer: %d\n",tam_buffer);

	send(socket_servidor,buffer,tam_buffer,0);

	free(buffer);
}

t_paquete recibir_paquete(int socket_cliente){
	int bytesRecibidos;
	t_paquete paquete;
	int tam_buffer;
	void* buffer;
	uint8_t cantidad_parametros = 0;
	//t_parametro* parametro;

	bytesRecibidos = recv(socket_cliente,&tam_buffer, sizeof(tam_buffer), 0);
	//bytesRecibidos = recv(socket_cliente,&paquete.header, sizeof(paquete.header), 0);
	paquete.error = 0;

	if(bytesRecibidos <= 0) {
		perror("Se desconecto el cliente");
		paquete.error = 1;
		return paquete;
	}

	buffer = malloc(tam_buffer); // solo para que el buffer no ocupe mucho

	recv(socket_cliente,buffer, sizeof(paquete.header), 0);
	memcpy(&paquete.header,buffer,sizeof(paquete.header));
	recv(socket_cliente,buffer, sizeof(cantidad_parametros), 0);
	memcpy(&cantidad_parametros,buffer,sizeof(cantidad_parametros));

	if(cantidad_parametros == 0){
		free(buffer);
		return paquete;
	}

	paquete.parametros = list_create();
	for(int i=0; i<cantidad_parametros ; i++){
		t_parametro* parametro = malloc(sizeof(t_parametro));

		// si hay un error, revisar lo del & ///////////////////////////////

		recv(socket_cliente,buffer, sizeof(parametro->valor), 0);
		memcpy(&parametro->valor,buffer,sizeof(parametro->valor));
		recv(socket_cliente,buffer, sizeof(parametro->recibir_string), 0);
		memcpy(&parametro->recibir_string,buffer,sizeof(parametro->recibir_string));

		////////////////////////////////////////////////////////////////////

		if(parametro->recibir_string){
			recv(socket_cliente,buffer, parametro->valor, 0);
			parametro->valor_string = malloc(parametro->valor);
			memcpy(parametro->valor_string,buffer,parametro->valor);
		}

		list_add(paquete.parametros,parametro);
	}

	free(buffer);

	return paquete;
}
