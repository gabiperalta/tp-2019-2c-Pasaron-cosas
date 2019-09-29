/*
 * biblioteca.h
 *
 *  Created on: 22 sep. 2019
 *      Author: utnso
 */

#ifndef BIBLIOTECA_H_
#define BIBLIOTECA_H_

#include <stdbool.h>
#include <commons/collections/list.h>

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
	//long unsigned int id_proceso_hilo;
	//int id_proceso_hilo;
	int error;
	//bool agregar_parametros
	t_list* parametros;

}t_paquete;

typedef struct{
	int valor;
	bool recibir_string;
	char* valor_string;
}t_parametro;

void agregar_valor(t_list* lista_parametros,int valor);
void agregar_string(t_list* lista_parametros,char* valor_string);

#endif /* BIBLIOTECA_H_ */
