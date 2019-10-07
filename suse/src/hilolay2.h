/*
 * hilolay.h
 *
 *  Created on: 6 oct. 2019
 *      Author: utnso
 */

#ifndef HILOLAY2_H_
#define HILOLAY2_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <semaphore.h>
#include <commons/collections/dictionary.h>

typedef struct{
	uint8_t id;
	sem_t mutex;
}hilolay_t;
void suse_create(hilolay_t thread,const hilolay_attr_t *attr,void*(void*),void*arg);

#endif /* HILOLAY2_H_ */
