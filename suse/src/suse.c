#include "suse.h"
#include "funcionesSuse.h"

int main(){
	puts("Proceso SUSE");
	leer_config();
	suse_log = log_create(PATH_LOG,"suse",false,LOG_LEVEL_INFO);
	inicializar_listas();
	inicializar_semaforos();
	pthread_create(&threadMetrics, NULL,(void*) metricas, NULL);
	inicializarServidor();
	iniciarPlanificacion();
	system("clear");
	log_info(suse_log,"====================== SUSE ======================");
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
	log_destroy(suse_log);
	pthread_join(threadMetrics, NULL);
	destructor_listas();
	destructor_semaforos();
	return 0;
}
