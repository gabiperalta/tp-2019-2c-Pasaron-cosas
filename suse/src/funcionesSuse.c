/*
 * funcionesSuse.c
 *
 *  Created on: 30 sep. 2019
 *      Author: utnso
 */

#include <funcionesSuse.h>

void inicializarColaNew(){
	//llegan los hilos con hilolay_create
		//pthread_t hilo;
		//pthread_create(&hilo;NULL;(void*)atenderRequest;NULL);
		cola_new = list_create();
		for(int i = 0; i < cola_new->elements_count; i++){
			list_add(cola_new, hilo);
		}

		free(hilo);
		return;
}
void inicializarColaReady(){
	cola_ready= list_create();

}
void pasarDeNewAReady(){
	int cantidad_max_procesos = obtenerGradoMultiprogramacion();
	for(int i = 0; i < cantidad_max_procesos; i++){

	}
}

int obtenerGradoMultiprogramacion(){
	t_config * config = obtenerConfigDeSuse();
	int grado_multiprog = config_get_int_value(config,"MAX_MULTIPROG");
	return grado_multiprog;
}

t_config * obtenerConfigDeSuse(){
	t_config * config = config_create(PATH_CONFIG);
	return config;
}


