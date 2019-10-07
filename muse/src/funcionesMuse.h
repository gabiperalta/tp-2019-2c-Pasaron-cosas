/*
 * funcionesMuse.h
 *
 *  Created on: 3 oct. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESMUSE_H_
#define FUNCIONESMUSE_H_

#include <commons/config.h>
#include "muse.h"

void procesar_solicitud(void* socket_cliente);
void leer_config();
void init_threads();
void servidor();

#endif /* FUNCIONESMUSE_H_ */
