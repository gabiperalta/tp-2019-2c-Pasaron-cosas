/*
 * biblioteca.c
 *
 *  Created on: 24 sep. 2019
 *      Author: utnso
 */

#include "biblioteca.h"

void agregar_valor(t_list* lista_parametros,int valor){
	t_parametro* parametro = malloc(sizeof(t_parametro));
	parametro->valor = valor;
	parametro->recibir_string = false;

	list_add(lista_parametros,parametro);
}

void agregar_string(t_list* lista_parametros,char* valor_string){
	t_parametro* parametro = malloc(sizeof(t_parametro));
	parametro->recibir_string = true;
	parametro->valor_string = strdup(valor_string);
	parametro->valor = strlen(parametro->valor_string);

	list_add(lista_parametros,parametro);
}
