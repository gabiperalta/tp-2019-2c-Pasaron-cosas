
#include "servidor.h"



void inicializarServidor(){
	extern int socketDeEscucha;
	t_config* config = config_create("/home/utnso/tp-2019-2c-Pasaron-cosas/sac-server/src/Config");
	int puertoDeEscucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	socketDeEscucha = escuchar(puertoDeEscucha);
	int socketCliente;

	printf("socketDeEscucha: %i\n", socketDeEscucha);
	pthread_t hiloServidor;



	//pthread_create(&hiloServidor,NULL,(void*)crearHiloDeAtencion, (void*)socketDeEscucha);
	//pthread_detach(hiloServidor);

	config_destroy(config);

}


void atenderRequest(int socketCliente){
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
		pthread_create(&hiloRequest,NULL,(void*)atenderRequest, (void*)socketCliente);
		pthread_detach(hiloRequest);

	}
}
