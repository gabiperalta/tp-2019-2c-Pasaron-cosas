/*
 * funcionesMuse.h
 *
 *  Created on: 3 oct. 2019
 *      Author: utnso
 */

#ifndef FUNCIONESMUSE_H_
#define FUNCIONESMUSE_H_

#include <commons/config.h>
#include <commons/string.h>
#include "muse.h"
#include "segmentacionPaginada.h"

void procesar_solicitud(void* socket_cliente);
void leer_config();
void init_threads();
void servidor();

void funcion_init(t_paquete paquete,int socket_muse);
void funcion_alloc(t_paquete paquete,int socket_muse);

char* obtener_ip_socket(int s);

#endif /* FUNCIONESMUSE_H_ */
