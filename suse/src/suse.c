#include "suse.h"

int main(){
	puts("Proceso SUSE");
	inicializar_listas();
	leer_config();
	inicializar_semaforos();
	iniciarLog();
	log_info(suse_log,"====================== SUSE ======================");
	inicializarServidor();
	log_info(suse_log, "hola");
	log_info(suse_log, "hasta aca llegue");
	iniciarPlanificacion();
	log_info(suse_log, "planifique");
	iniciarMetricas();
	log_info(suse_log, "metricas");
	system("clear");
	printf("---------------- SUSE ----------------\n");
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

	//destructor_listas();
	destruirLog();
	destructor_semaforos_globales();

	liberarCharAsteriscoAsterisco(ids_sem);
	liberarCharAsteriscoAsterisco(inicio_sem);
	liberarCharAsteriscoAsterisco(max_sem);
	return 0;
}
