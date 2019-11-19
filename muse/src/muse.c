#include "muse.h"


int main(void) {

	leer_config();
	lista_procesos = list_create();
	lista_clock = list_create();
	lista_archivos_mmap = list_create();

	init_memoria();
	init_threads();

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
