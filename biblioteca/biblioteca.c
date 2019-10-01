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
	parametro->valor = strlen(parametro->valor_string) + 1; // se hace + 1 por el \0

	list_add(lista_parametros,parametro);
}

int obtener_valor(t_list* lista_parametros){
	t_parametro* parametro = list_get(lista_parametros,0);
	int valor_obtenido = parametro->valor;

	list_remove_and_destroy_element(lista_parametros,0,(void*) destruir_parametro);

	return valor_obtenido;
}

char* obtener_string(t_list* lista_parametros){
	t_parametro* parametro = list_get(lista_parametros,0);
	char* string_obtenido = malloc(parametro->valor);
	strcpy(string_obtenido,parametro->valor_string);

	list_remove_and_destroy_element(lista_parametros,0,(void*) destruir_parametro);

	return string_obtenido;
}

void destruir_parametro(t_parametro* parametro){
	if(parametro->recibir_string)
		free(parametro->valor_string);
	free(parametro);
}
