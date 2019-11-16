/*
 * hilolay_alumnos.c
 *
 *  Created on: 12 nov. 2019
 *      Author: utnso
 */


	typedef struct hilolay_operations {
		.suse_create= me_create,
		.suse_schedule_next = me_schedule_next,
		// aca todas, las me_ le mandan los datos a funciones suse por el socket.
	} hilolay_operations;


	int me_create(int tid){
	t_paquete paquete_respuesta = {
		.header = SUSE_CREATE,
		.parametros = list_create()
		};

	agregar_valor(paquete_respuesta.parametros,tid);

	enviar_paquete(paquete_respuesta,socket) //el que guarde al inciiar la conex)

	t_paquete paquete_respuesta = recibir_paquete(socket);

	int retorno = obtener_valor(paquete.parametros) //la de funciones suse, lo que te retorna

	return retorno} //ponele un 0 si esta ok


