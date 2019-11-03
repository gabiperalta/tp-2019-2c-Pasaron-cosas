/*
 * biblioteca.h
 *
 *  Created on: 22 sep. 2019
 *      Author: utnso
 */

#ifndef BIBLIOTECA_H_
#define BIBLIOTECA_H_

#include <stdbool.h>
#include <stdint.h>
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
	MUSE_UNMAP,
	MUSE_INIT_THREAD,
	SUSE_INIT,
	SUSE_CREATE,
	SUSE_SCHEDULE_NEXT,
	SUSE_WAIT,
	SUSE_SIGNAL,
	SUSE_JOIN,
	SUSE_CLOSE,
	FUSE_INIT,
	FUSE_GETATTR,
	FUSE_READDIR,
	FUSE_MKNOD,
	FUSE_OPEN,
	FUSE_WRITE,
	FUSE_READ,
	FUSE_UNLINK,
	FUSE_MKDIR,
	FUSE_RMDIR
} funcion;

typedef struct{

	funcion header;
	int error; // es 1 cuando se desconecta el cliente
	//bool agregar_parametros
	t_list* parametros;

}t_paquete;

typedef struct{
	uint32_t valor;
	bool recibir_string;
	char* valor_string;
}t_parametro;

void agregar_valor(t_list* lista_parametros,uint32_t valor);
void agregar_string(t_list* lista_parametros,char* valor_string);
uint32_t obtener_valor(t_list* lista_parametros);
char* obtener_string(t_list* lista_parametros);
void destruir_parametro(t_parametro* parametro);

#endif /* BIBLIOTECA_H_ */
