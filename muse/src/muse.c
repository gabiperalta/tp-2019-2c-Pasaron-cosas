#include "muse.h"


int main(void) {

	//int (*funcion_muse)(int);
	//funcion_muse = muse_prueba;
	//funcion_muse(16);
	//return 0;


	leer_config();
	lista_threads = list_create();

	upcm = malloc(TAM_MEMORIA); // memoria principal

	init_threads();


	/*
	int cliente;
	int puerto_escucha;
	t_paquete paquete_recibido;

	puerto_escucha = escuchar(PUERTO);
	cliente = aceptarConexion(puerto_escucha);

	paquete_recibido = recibir_paquete(cliente);

	printf("string recibido: %s\n",obtener_string(paquete_recibido.parametros));

	close(cliente);
	*/

	//printf("%d\n",PUERTO);
	//printf("%d\n",TAM_MEMORIA);
	//printf("%d\n",TAM_PAGINA);
	//printf("%d\n",TAM_SWAP);

	system("clear");
	printf("---------------- MUSE ----------------\n");

	char * linea;
	while(1) {
		linea = readline(">");
		if(linea)
			add_history(linea);
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}

		free(linea);
	}

	free(upcm);
	return 0;
}
