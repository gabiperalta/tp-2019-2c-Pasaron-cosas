#include <suse.h>

int main(){
	puts("Proceso SUSE");
	leer_config();
	suse_log = log_create(PATH_LOG,"suse",false,LOG_LEVEL_INFO);
	lista_procesos = list_create();
	hilos_new = list_create();
	hilos_blocked = list_create();
	hilos_exit = list_create();
	semaforos = list_create();
	sem_init(sem_planificacion,0, grado_multiprogramacion);
	sem_init(sem_join,NULL,1);
	sem_init(sem_ejecute,NULL,1);
	pthread_create(&threadMetrics, NULL, metrics, NULL);
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
	list_destroy_and_destroy_elements(lista_procesos,(void*)destructor_de_procesos);
	list_destroy(hilos_new);
	list_destroy(hilos_blocked);
	list_destroy(hilos_exit);
	list_destroy_and_destroy_elements(semaforos,(void*)destructor_de_semaforos);
	sem_destroy(sem_planificacion);
	sem_destroy(sem_join);
	sem_destroy(sem_ejecute);
	return 0;
}
