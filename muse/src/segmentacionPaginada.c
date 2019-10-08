/*
 * segmentacionPaginada.c
 *
 *  Created on: 5 oct. 2019
 *      Author: utnso
 */

#include "segmentacionPaginada.h"


t_thread* crear_thread(char* id_proceso_hilo,int socket_creado){
	t_thread* nuevo = malloc(sizeof(t_thread));

	nuevo->id_proceso_hilo = strdup(id_proceso_hilo);
	nuevo->socket = socket_creado;
	nuevo->tabla_segmentos = list_create();

	return nuevo;
	//list_add(lista_threads,nuevo);
}


t_segmento* crear_segmento(uint8_t tipo) {
	 t_segmento* nuevo = malloc(sizeof(t_segmento));
	 nuevo->tipo_segmento = tipo;
	 nuevo->tabla_paginas = list_create();
	 return nuevo;
}


t_pagina* crear_pagina(uint8_t bit_modificado, void* datos) {
    t_pagina* new = malloc(sizeof(t_pagina));
    //new->numeroPagina = numeroPagina;
    new->bit_modificado = bit_modificado;
    //new->marco = guardarRegistro(registro);
    return new;
}

t_thread* buscar_thread(t_list* lista,int socket_thread) {
	int igualSocket(t_thread *p) {
		return p->socket == socket_thread;
	}

	return list_find(lista, (void*) igualSocket);
}

/*
t_registro* crearRegistro(t_response respuesta, uint16_t keyNuevo){
	t_registro* nuevo = malloc(sizeof(t_registro));
	nuevo->key = keyNuevo;
	nuevo->timestamp = respuesta.timestamp;
	nuevo->value = respuesta.value ;
	return nuevo;
}

t_pagina* buscarPagina(t_list* lista,uint16_t key) {
	int igualKey(t_pagina* p){

		uint16_t keyEncontrada;
		void* direccion = p->direccion;

		memcpy(&keyEncontrada,(char*)direccion + sizeof(int), sizeof(keyEncontrada));

		if(keyEncontrada == key){
			return 1;
		}
		else{
			return 0;
		}
	}

	return list_find(lista, (void*) igualKey);
}

t_pagina* buscarPaginaPorNumero(t_list* lista, int numeroPagina) {
	int igualNumero(t_pagina* p){
		return p->numeroPagina == numeroPagina;
	}
	return list_find(lista, (void*) igualNumero);
}

void* guardarRegistro(t_registro registro){
	int posicionAnalizada = 0;
	void * direccion;
	int posicion = 0;
	int lugarVacio = 0;

	while(lugarVacio != 1){
		direccion = (char*) memoria + posicionAnalizada;

		if(obtenerTimestamp(direccion) == 0){
			memcpy(direccion,&registro.timestamp,sizeof(registro.timestamp));
			posicion += sizeof(registro.timestamp);
			memcpy(&direccion[posicion],&registro.key,sizeof(registro.key));
			posicion += sizeof(registro.key);
			memcpy(&direccion[posicion],registro.value,strlen(registro.value) + 1);
			//posicion += VALUE;

			lugarVacio = 1;

			return direccion;
		}

		//posicionAnalizada += MAX_VALUE + sizeof(registro.timestamp) + sizeof(registro.key);
		posicionAnalizada += max_value + sizeof(registro.timestamp) + sizeof(registro.key);
	}
}

void actualizarRegistro(t_pagina* pagina,t_registro registro){
	int posicion = 0;
	//int posicion = sizeof(registro.timestamp) + sizeof(registro.key);
	void* direccion = pagina->direccion;

	memset(direccion,0,sizeof(registro.timestamp));
	memcpy(direccion,&registro.timestamp,sizeof(registro.timestamp));
	posicion += sizeof(registro.timestamp);

	memset(&direccion[posicion],0,sizeof(registro.key));
	memcpy(&direccion[posicion],&registro.key,sizeof(registro.key));
	posicion += sizeof(registro.key);

	//memset(&direccion[posicion],0,MAX_VALUE);
	memset(&direccion[posicion],0,max_value);
	memcpy(&direccion[posicion],registro.value,strlen(registro.value)+1);
}

char* obtenerValue(void* direccion){
	//char* value = malloc(MAX_VALUE);
	char* value = malloc(max_value);
	t_registro registro;

	//memcpy(value,(char*)direccion + sizeof(registro.timestamp) + sizeof(registro.key),MAX_VALUE);
	memcpy(value,(char*)direccion + sizeof(registro.timestamp) + sizeof(registro.key),max_value);

	return value;
}

uint16_t obtenerKey(void* direccion){
	uint16_t key;
	t_registro registro;

	memcpy(&key,(char*)direccion + sizeof(registro.timestamp),sizeof(registro.key));

	return key;
}

uint32_t obtenerTimestamp(void* direccion){
	//int* timestamp = malloc(30);
	uint32_t timestamp = 0;

	memcpy(&timestamp,(char*)direccion,sizeof(uint32_t));

	return timestamp;
}

int obtenerIndicePagina(t_list* lista){
	if(list_size(lista)){
		t_pagina* pagina_obtenida = list_get(lista,list_size(lista)-1);
		return pagina_obtenida->numeroPagina + 1;
	}
	else{
		return 1;
	}
}

uint32_t getCurrentTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

/////////////////////////////JOURNAL//////////////////////////////////
t_registroJOURNAL* crearRegistroJOURNAL(char* path, t_pagina* pagina) {

	t_registroJOURNAL* nuevo = malloc(sizeof(t_registroJOURNAL));
	 nuevo->path = strdup(path);
	 nuevo->value = obtenerValue(pagina->direccion);
	 nuevo->timestamp = obtenerTimestamp(pagina->direccion);

	 return nuevo;
}

void filtrarModificados(t_list* listaOriginal, t_list* tabla_segmento){
	bool estaModificada(t_pagina* p){
			int flagEncontrada;
			void* direccion = p->direccion;

			memcpy(&flagEncontrada,(char*)direccion + sizeof(int), sizeof(flagEncontrada));

			if(flagEncontrada == 1){
				return 1;
			}
			else{
				return 0;
			}
		}
	t_list* listaSecundaria = list_filter(tabla_segmento,(void*) estaModificada );
	list_add_all(listaOriginal,listaSecundaria);
}

t_list* quePasarEnElJournal(t_list* tabla_segmentos){

	t_list* listaDeRegistros;
	for(int i = 0; i<tabla_segmentos->elements_count; i++){

		filtrarModificados(list_get(tabla_segmentos,i), listaDeRegistros);

		}
	return listaDeRegistros;
}

t_pagina* buscarPaginaModificadaONO(t_list* lista, int flag) {
	int estaModificada(t_pagina* p, int flag){
		int flagEncontrada;
		void* direccion = p->direccion;

		memcpy(&flagEncontrada,(char*)direccion + sizeof(int), sizeof(flagEncontrada));

		if(flagEncontrada == flag){
			return 1;
		}
		else{
			return 0;
		}
	}
	return list_find(lista, (void*) estaModificada);
}

////////////COSAS PARA EL LRU//////////////////

t_registro_LRU* crearRegistroLRU(char* path, t_pagina* pagina) {
	 t_registro_LRU* nuevo = malloc(sizeof(t_registro_LRU));
	 nuevo->path = strdup(path);
	 nuevo->numeroPagina = pagina->numeroPagina;
	 nuevo->modificado = pagina->modificado;
	 return nuevo;
}

t_registro_LRU* buscarRegistroLRU(char *path, t_pagina* pagina) {

	int igualRegistro(t_registro_LRU* p) {
		return string_equals_ignore_case(p->path, path) && (p->numeroPagina == pagina->numeroPagina);
	}

	return list_find(lista_LRU, (void*) igualRegistro);
}

void agregarEnListaLRU(char* path, t_pagina* page){

	if (!buscarRegistroLRU(path,page)){ //si no esta
		list_add(lista_LRU, crearRegistroLRU(path, page));
	}
	else {
		int igualRegistro(t_registro_LRU* p) {
			return string_equals_ignore_case(p->path, path) && (p->numeroPagina == page->numeroPagina);
		}
		list_remove_and_destroy_by_condition(lista_LRU,(void*) igualRegistro,(void*) eliminarRegistroLRU);

		list_add(lista_LRU, crearRegistroLRU(path, page));
	}
}

void eliminarRegistroLRU(t_registro_LRU* registro_LRU){
	free(registro_LRU->path);
	free(registro_LRU);
}

int liberarMemoriaLRU(){
	t_registro_LRU* registroLRU_encontrado;

	int sinModificar(t_registro_LRU* p){
		return p->modificado == 0;
	}

	registroLRU_encontrado = list_find(lista_LRU,(void*) sinModificar);

	if(registroLRU_encontrado != NULL){
		t_segmento* segmento_encontrado = buscarSegmento(tabla_segmentos,registroLRU_encontrado->path);
		t_pagina* pagina_encontrada = buscarPaginaPorNumero(segmento_encontrado->tabla_pagina,registroLRU_encontrado->numeroPagina);

		int igualNumero(t_pagina* p){
			return p->numeroPagina == pagina_encontrada->numeroPagina;
		}
		list_remove_and_destroy_by_condition(segmento_encontrado->tabla_pagina,(void*) igualNumero,(void*) eliminarPagina);
		list_remove_and_destroy_by_condition(lista_LRU,(void*) sinModificar,(void*) eliminarRegistroLRU);

		cantPaginasLibres++;

		return 1;
	}
	else{
		// estan todas las paginas ocupadas
		return 0;
	}
}

void dropSegmento(t_segmento* segment){
	cantPaginasLibres += cantidadDePaginasEliminadas(segment);

	int igualPath(t_segmento* p) {
		return string_equals_ignore_case(p->path, segment->path);
	}
	list_remove_and_destroy_by_condition(tabla_segmentos,(void*) igualPath,(void*) eliminarSegmento);

	t_registro_LRU* registro_LRU;
	for(int i=0; i<list_size(lista_LRU); i++){
		registro_LRU = list_get(lista_LRU,i);
		int igualPathLRU(t_registro_LRU* p) {
			return string_equals_ignore_case(p->path, registro_LRU->path);
		}
		list_remove_and_destroy_by_condition(lista_LRU,(void*) igualPathLRU,(void*) eliminarRegistroLRU);
	}

}

void eliminarSegmento(t_segmento* segment){
	free(segment->path);
	list_destroy_and_destroy_elements(segment->tabla_pagina,(void*) eliminarPagina);
	free(segment);
}

void eliminarPagina(t_pagina* pagina){
	memset(pagina->direccion,NULL,tamano_registro);
	free(pagina);
}

int cantidadDePaginasEliminadas(t_segmento* segment){
	return list_size(segment->tabla_pagina);
}

void vaciarMemoria(){

	list_clean_and_destroy_elements(tabla_segmentos,(void*) eliminarSegmento);

	//list_fold(segment->tabla_pagina, 0 , (void*) eliminarSegmento);
	//tambien vacia auxLRU
	// la cambie porque no vaciaba la memo, si no es eso, es el destructor

	// comento la funcion porque me da error
	//list_remove_and_destroy_element(auxLRU,(void*)destructorDeSegmentoAUX);
}
*/
