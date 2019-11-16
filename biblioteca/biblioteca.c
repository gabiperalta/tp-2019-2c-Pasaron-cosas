/*
 * biblioteca.c
 *
 *  Created on: 24 sep. 2019
 *      Author: utnso
 */

#include "biblioteca.h"

void agregar_valor(t_list* lista_parametros,uint32_t valor){
	t_parametro* parametro = malloc(sizeof(t_parametro));
	parametro->valor = valor;
	parametro->recibir_bloque_datos = false;

	list_add(lista_parametros,parametro);
}

void agregar_string(t_list* lista_parametros,char* valor_string){
	t_parametro* parametro = malloc(sizeof(t_parametro));
	parametro->recibir_bloque_datos = true;
	parametro->bloque_datos = strdup(valor_string);
	parametro->valor = strlen(parametro->bloque_datos) + 1; // se hace + 1 por el \0

	list_add(lista_parametros,parametro);
}

void agregar_bloque_datos(t_list* lista_parametros,void* bloque_datos, uint32_t tam_bloque_datos){
	t_parametro* parametro = malloc(sizeof(t_parametro));
	parametro->recibir_bloque_datos = true;
	parametro->valor = tam_bloque_datos;
	parametro->bloque_datos = malloc(tam_bloque_datos);

	memcpy(parametro->bloque_datos,bloque_datos,tam_bloque_datos);

	list_add(lista_parametros,parametro);
}

uint32_t obtener_valor(t_list* lista_parametros){
	t_parametro* parametro = list_get(lista_parametros,0);
	int valor_obtenido = parametro->valor;

	list_remove_and_destroy_element(lista_parametros,0,(void*) destruir_parametro);

	return valor_obtenido;
}

char* obtener_string(t_list* lista_parametros){
	t_parametro* parametro = list_get(lista_parametros,0);
	char* string_obtenido = malloc(parametro->valor);

	strcpy(string_obtenido,parametro->bloque_datos);

	list_remove_and_destroy_element(lista_parametros,0,(void*) destruir_parametro);

	return string_obtenido;
}

void* obtener_bloque_datos(t_list* lista_parametros){
	t_parametro* parametro = list_get(lista_parametros,0);
	void* bloque_datos_obtenido = malloc(parametro->valor);

	memcpy(bloque_datos_obtenido,parametro->bloque_datos,parametro->valor);

	printf("parametro->valor %d\n",parametro->valor);
	printf("bloque_datos_obtenido biblioteca: %s\n",bloque_datos_obtenido);

	list_remove_and_destroy_element(lista_parametros,0,(void*) destruir_parametro);

	return bloque_datos_obtenido;
}

void destruir_parametro(t_parametro* parametro){
	if(parametro->recibir_bloque_datos)
		free(parametro->bloque_datos);
	free(parametro);
}
