

#include "suse.h"


int main(){
	estimacion_inicial = 0;
	puts("Proceso SUSE");
	inicializar_listas();
	inicializar_semaforos();
<<<<<<< HEAD
	leer_config();
	suse_log = log_create(PATH_LOG,"suse",false,LOG_LEVEL_INFO);
	pthread_create(&threadMetrics, NULL,(void*) metricas, NULL);
=======
	//ACA PONES METRICAS Y NO ESTA DECLARADO EN NINGUN LADO
	//pthread_create(&threadMetrics, NULL,(void*) metricas, NULL);
>>>>>>> 76dff580cd951483a9e301535d84528c812a2ddc
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
	//aca no tiene que ser un puntero el detach
	//pthread_detach(&threadMetrics);
	destructor_listas();
	destructor_semaforos();
	return 0;
}
