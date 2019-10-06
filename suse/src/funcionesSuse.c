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
	// hay que chequear aca constantemente si se libera espacio para que de new pasen a ready?
}
void pasarDeNewAReady(){
	int cantidad_max_procesos = obtenerGradoMultiprogramacion();

	for(int i = 0; cola_new->elements_count ; i++){
		if(cola_ready->elements_count <= cantidad_max_procesos){

			list_add(cola_ready, cola_new[i]);
			list_remove(cola_new, cola_new[i]);
			//ver el tema de usar SJF a corto plazo


		}
		else{
			break; //se me ocurre esto para que si en el medio del for se libera un cupo de ready no se asigne
				//un hilo de new que no esta primero en la lista
				//es decir, si ya vemos que la lista de ready esta llena que salga de la asignacion
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
