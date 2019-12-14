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

// Si no se envia la lista de parametros, inicializarla con NULL
void enviar_paquete(t_paquete paquete,int socket_servidor){
	void* buffer;
	int posicion = 0;
	uint8_t cantidad_parametros = 0;
	int tam_buffer = sizeof(paquete.header) + sizeof(cantidad_parametros);
	t_parametro* parametro;

	if(paquete.parametros != NULL){
		cantidad_parametros = list_size(paquete.parametros);

		for(int i=0;i<cantidad_parametros;i++){
			parametro = list_get(paquete.parametros,i);
			tam_buffer += sizeof(parametro->valor) + sizeof(parametro->recibir_bloque_datos);
			if(parametro->recibir_bloque_datos)
				tam_buffer += parametro->valor;
		}
	}

	send(socket_servidor,&tam_buffer,sizeof(tam_buffer),0);

	void* buffer_aux = malloc(sizeof(tam_buffer));
	int total_recibido = 0;
	int cant_a_recibir = sizeof(tam_buffer);
	int tam_buffer_retornado;
	int bytesRecibidos;
	do{
		bytesRecibidos = recv(socket_servidor, &buffer_aux[total_recibido], cant_a_recibir - total_recibido, 0);
		total_recibido += bytesRecibidos;
	}while(total_recibido != cant_a_recibir && bytesRecibidos > 0);
	memcpy(&tam_buffer_retornado,buffer_aux,sizeof(tam_buffer));
	if(tam_buffer == tam_buffer_retornado)
		printf("El envio de datos se realizara bien\n");
	free(buffer_aux);

	buffer = malloc(tam_buffer);

	//printf("TAM BUFFER enviar_paquete: %i\n", tam_buffer);
	memcpy(&buffer[posicion],&paquete.header,sizeof(paquete.header));
	posicion += sizeof(paquete.header);
	memcpy(&buffer[posicion],&cantidad_parametros,sizeof(cantidad_parametros));
	posicion += sizeof(cantidad_parametros);

	if(paquete.parametros != NULL){
		for(int i=0;i<cantidad_parametros;i++){
			parametro = list_get(paquete.parametros,i);

			memcpy(&buffer[posicion],&parametro->valor,sizeof(parametro->valor));
			posicion += sizeof(parametro->valor);

			memcpy(&buffer[posicion],&parametro->recibir_bloque_datos,sizeof(parametro->recibir_bloque_datos));
			posicion += sizeof(parametro->recibir_bloque_datos);

			if(!parametro->recibir_bloque_datos){ continue; }
			memcpy(&buffer[posicion],parametro->bloque_datos,parametro->valor);
			posicion += parametro->valor;

		}
		list_destroy_and_destroy_elements(paquete.parametros,(void*) destruir_parametro);
	}

	int bytesEnviados = send(socket_servidor,buffer,tam_buffer,0);

	printf("bytesEnviados %d\n",bytesEnviados);

	free(buffer);
}

t_paquete recibir_paquete(int socket_cliente){
	int bytesRecibidos;
	int posicion = 0;
	t_paquete paquete;
	int tam_buffer;
	void* buffer;
	uint8_t cantidad_parametros = 0;

	int total_recibido = 0;
	int cant_a_recibir = sizeof(tam_buffer);

	//bytesRecibidos = recv(socket_cliente,&tam_buffer, sizeof(tam_buffer), 0);
	buffer = malloc(cant_a_recibir);
	//bytesRecibidos = recv(socket_cliente,buffer,200000,0);
	paquete.error = 0;

	do{
		bytesRecibidos = recv(socket_cliente, &buffer[total_recibido], cant_a_recibir - total_recibido, 0);
		total_recibido += bytesRecibidos;
	}while(total_recibido != cant_a_recibir && bytesRecibidos > 0);

	if(bytesRecibidos <= 0) {
		perror("Se desconecto el cliente");
		paquete.error = 1;
		free(buffer);
		return paquete;
	}

	send(socket_cliente,buffer,sizeof(tam_buffer),0); // envio esto para decir que esta _todo ok

	memcpy(&tam_buffer,buffer,sizeof(tam_buffer));
	buffer = realloc(buffer,tam_buffer);
	total_recibido = 0;
	cant_a_recibir = tam_buffer;
	do{
		bytesRecibidos = recv(socket_cliente, &buffer[total_recibido], cant_a_recibir - total_recibido, 0);
		total_recibido += bytesRecibidos;
	}while(total_recibido != cant_a_recibir && bytesRecibidos > 0);

	//buffer = malloc(tam_buffer); // solo para que el buffer no ocupe mucho
	if(buffer == NULL){
		printf("\tmalloc demasiado grande\n");
	}

	//printf("BYTES RECIBIDOS: %i\n", bytesRecibidos);
	//printf("TAM BUFFER recibir_paquete: %i\n", tam_buffer);
	//posicion = sizeof(tam_buffer);

	//recv(socket_cliente,buffer, sizeof(paquete.header), 0);
	memcpy(&paquete.header,&buffer[posicion],sizeof(paquete.header));
	posicion += sizeof(paquete.header);
	printf("paquete.header: %d\n", paquete.header);
	//recv(socket_cliente,buffer, sizeof(cantidad_parametros), 0);
	memcpy(&cantidad_parametros,&buffer[posicion],sizeof(cantidad_parametros));
	posicion += sizeof(cantidad_parametros);
	printf("cantidad_parametros: %d\n", cantidad_parametros);

	if(cantidad_parametros == 0){
		free(buffer);
		return paquete;
	}

	paquete.parametros = list_create();
	for(int i=0; i<cantidad_parametros ; i++){
		t_parametro* parametro = malloc(sizeof(t_parametro));

		//recv(socket_cliente,buffer, sizeof(parametro->valor), 0);
		memcpy(&parametro->valor,&buffer[posicion],sizeof(parametro->valor));
		posicion += sizeof(parametro->valor);
		//recv(socket_cliente,buffer, sizeof(parametro->recibir_bloque_datos), 0);
		memcpy(&parametro->recibir_bloque_datos,&buffer[posicion],sizeof(parametro->recibir_bloque_datos));
		posicion += sizeof(parametro->recibir_bloque_datos);

		if(parametro->recibir_bloque_datos){
			//recv(socket_cliente,buffer, parametro->valor, 0);
			parametro->bloque_datos = malloc(parametro->valor);
			memcpy(parametro->bloque_datos,&buffer[posicion],parametro->valor);
			posicion += parametro->valor;
		}

		list_add(paquete.parametros,parametro);
	}

	free(buffer);

	return paquete;
}
