/*
 * biblioteca.h
 *
 *  Created on: 22 sep. 2019
 *      Author: utnso
 */

#ifndef BIBLIOTECA_H_
#define BIBLIOTECA_H_

typedef enum {
	MUSE_ALLOC = 1,
	MUSE_INIT,
	MUSE_CLOSE,
	MUSE_FREE,
	MUSE_GET,
	MUSE_CPY,
	MUSE_MAP,
	MUSE_SYNC,
	MUSE_UNMAP
} funcion;

typedef struct{

	funcion header;
	//uint8_t tam_ip;
	//char* ip;
	long unsigned int id_proceso_hilo;
	int error;

}t_datos;

#endif /* BIBLIOTECA_H_ */
