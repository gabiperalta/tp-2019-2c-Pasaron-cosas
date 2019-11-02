
#include "servidor.h"



void inicializarServidor(){

	printf("socketDeEscucha: %i\n", PUERTO);
	pthread_t hiloServidor;

	pthread_create(&hiloServidor,NULL,(void*)servidor, NULL);
	pthread_detach(hiloServidor);
}

void servidor(){
	void * conectado;
	int puerto_escucha = escuchar(PUERTO);

	while((conectado=aceptarConexion(puerto_escucha))!= 1){

		//printf("Se acepto conexion\n");
		pthread_t thread_solicitud;
		pthread_create(&thread_solicitud,NULL,(void*)procesar_solicitud,conectado);
		pthread_detach(thread_solicitud);
	}
}


void hiloDeAtencionProceso(int socketCliente){

	t_list *listaDeProcesosAbiertos = list_create();
	//t_request request = recibirRequest(socketCliente);
	//int horror;
	//t_response structRespuesta;


	//switch(request.header){
	///////////////ACA VAN A IR LOS DISTINTOS CASOS DE LECTURA DEPENDIENDO DEL HEADER DE LA REQUEST
	//}

	//liberarMemoriaRequest(request);

	close(socketCliente);

	return;
}

///  CON ESTA IMPLEMENTACION SE RESUELVEN LAS DISTINTAS REQUEST EN PARALELO, NO SE CUAL ES EL REQUERIMIENTO SOBRE ESE TEMA

void crearHiloDeAtencion(int listenningSocket){
	int socketCliente;
	while((socketCliente=aceptarConexion(listenningSocket))!= 1){
		pthread_t hiloRequest;
		pthread_create(&hiloRequest,NULL,(void*)hiloDeAtencionProceso, (void*)socketCliente);
		pthread_detach(hiloRequest);

	}
}
