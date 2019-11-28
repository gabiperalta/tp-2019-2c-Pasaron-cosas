#include "muse.h"


int main(void) {

	init_memoria();
	init_threads();

	log_info(archivo_log,"====================== MUSE ======================");

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

	log_destroy(archivo_log);
	free(upcm);
	return 0;
}
