#include <suse.h>

int main(){
	puts("Proceso SUSE");
	leer_config();
	lista_procesos = list_create();
	hilos_new = list_create();
	hilos_blocked = list_create();
	hilos_exit = list_create();
	semaforos = list_create();
	sem_init(sem_planificacion,0, grado_multiprogramacion);
	iniciarPlanificacion();
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
	list_destroy_and_destroy_elements(lista_procesos,(void*)destructor_de_procesos);
	list_destroy(hilos_new);
	list_destroy(hilos_blocked);
	list_destroy(hilos_exit);
	list_destroy_and_destroy_elements(semaforos,(void*)destructor_de_semaforos);
	return 0;
}
