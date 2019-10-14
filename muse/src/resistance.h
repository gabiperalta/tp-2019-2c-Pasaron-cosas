/*
 * funcionesMuse.h
 *
 *  Created on: 3 oct. 2019
 *      Author: utnso
 */

#ifndef RESISTANCE_H_
#define RESISTANCE_H_

#include <commons/config.h>
#include <commons/string.h>
#include "muse.h"
#include "uprising.h"

void procesar_solicitud(void* socket_cliente);
void leer_config();
void init_threads();
void servidor();

void funcion_init(t_paquete paquete,int socket_muse);
void funcion_alloc(t_paquete paquete,int socket_muse);

char* obtener_ip_socket(int s);

#endif /* RESISTANCE_H_ */
