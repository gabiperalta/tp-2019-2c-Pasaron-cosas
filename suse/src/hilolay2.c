/*
 * hilolay2.c
 *
 *  Created on: 6 oct. 2019
 *      Author: utnso
 */
#include <hilolay2.h>

void suse_create(hilolay_t thread,const hilolay_attr_t *p,void*funcion(void*),void*parametros){
	thread = init_thread();
	sem_init(thread->mutex,NULL,0);
	dictionary_put(cola_new,thread->id,thread);
	//enviar_hilo(thread);
	sem_wait(thread->mutex); // aca se van a bloquear los hilos
	if(!p){
		*p = funcion(parametros);
	}
}

