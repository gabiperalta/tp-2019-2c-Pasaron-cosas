#include "suse.h"

int main(){
	estimacion_inicial = 0;
	puts("Proceso SUSE");
	inicializar_listas();
	inicializar_semaforos();
	leer_config();
	iniciarLog();
	log_info(suse_log,"====================== SUSE ======================");
	inicializarServidor();
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
	destruirLog();
	destruirMetricas();
	destructor_listas();
	destructor_semaforos();
	return 0;
}
