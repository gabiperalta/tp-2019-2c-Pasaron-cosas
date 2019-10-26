#include "funcionesSac.h"


//////// FUNCIONES PRINCIPALES ////////

// FUNCIONES GENERALES








// FUNCIONES CON DIRECTORIOS

// MKDIR
int crearDirectorio(const char *path, mode_t mode){ // mode ni lo usamos
	GFile *directorioPadre;
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO, directorioPadre);
	char** pathDividida = string_split(path, '/');
	int longitudDePath = cantidadElementosCharAsteriscoAsterisco(pathDividida);
	ptrGBloque numeroInodo = reservarInodo(DIRECTORIO);
	GFile *inodo;
	struct timeval tiempo;

	gettimeofday(tiempo, NULL);

	if(directorioPadre == NULL){
		return -1;
	}

	if(obtenerEntrada(directorioPadre)){ // si retorna 1 entonces se pudo asignar la entrada
		if(numeroInodo > 0){ // DEBERIA SINCRONIZAR ESTO DE MANERA QUE UNA VEZ QUE SEPA QUE TIENE INODO Y ENTRADA DI DIRECTORIO LIBRES, NADIE SE LOS PUEDA QUITAR
			inodo = obtenerBloque(numeroInodo); // PUEDO ASIGNARLO Y SI NO PUEDO TERMINAR LA OPREACION LO LIBERA
			for(int i; i<1000; i++){
				inodo->blocks[i] = 0;
			}


			inodo->creation_date = tiempo.tv_usec;
			inodo->father_block = punteroAInodoPadre;
			inodo->file_size = 0;
			memcpy(inodo->fname, pathDividida[longitudDePath - 1]);
			inodo->modification_date = tiempo.tv_usec;
			inodo->state = DIRECTORIO;

			liberarCharAsteriscoAsterisco(pathDividida);

			return 0;

		}
		borrarEntrada(directorioPadre);
	}
	return -1;
}

// RMDIR
int eliminarDirectorio(const char *path){
	GFile *directorio;
	ptrGBloque punteroAInodo = buscarInodoArchivo(path, SIN_EL_ULTIMO, directorio);

	if(punteroAInodo != 0){
		if(noTieneHijos(punteroAInodo)){

			liberarBloquesAsignados(directorio->blocks);
			directorio->state = BORRADO;

			borrarEntrada();

			return 0;

		}
	}

	return -1;
}

// LS
// readdir();

// FUNCIONES ARCHIVOS
// MKNOD
int crearArchivo(const char *path, mode_t, dev_t){ // no usamos ni mode ni dev
	GFile *directorioPadre;
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO, directorioPadre);
	char** pathDividida = string_split(path, '/');
	int longitudDePath = cantidadElementosCharAsteriscoAsterisco(pathDividida);
	ptrGBloque numeroInodo = reservarInodo(ARCHIVO);
	GFile *inodo;
	struct timeval tiempo;

	gettimeofday(tiempo, NULL);

	if(directorioPadre == NULL){
		return -1;
	}

	if(tieneEntradaLibre(directorioPadre)){
		if(inodoLibre > 0){ // DEBERIA SINCRONIZAR ESTO DE MANERA QUE UNA VEZ QUE SEPA QUE TIENE INODO Y ENTRADA DI DIRECTORIO LIBRES, NADIE SE LOS PUEDA QUITAR
			inodo = obtenerBloque(numeroInodo);	// PUEDO ASIGNARLO Y SI NO PUEDO TERMINAR LA OPREACION LO LIBERA
			for(int i; i<1000; i++){
				inodo->blocks[i] = 0;
			}


			inodo->creation_date = tiempo.tv_usec;
			inodo->father_block = punteroAInodoPadre;
			inodo->file_size = 0;
			memcpy(inodo->fname, pathDividida[longitudDePath - 1]);
			inodo->modification_date = tiempo.tv_usec;
			inodo->state = DIRECTORIO;

			liberarCharAsteriscoAsterisco(pathDividida);

			return 0;
		}
	}
	return -1;
}

// OPEN
int abrirArchivo(const char *path, struct fuse_file_info *){
	// REVISAR LA TABLA DE ARCHIVOS ABIERTOS A VER SI EL ARCHIVO YA LO ESTA

	// SI LO ESTA, DIRECTAMENTE SE PASA LA POSICION DE SU FD DENTRO DE LA LISTA

	// SINO, SE CARGA EN LA LISTA, Y RECIEN AHI SE PASA LA POSICION
}


// UNLINK
int eliminarArchivo(const char *path){

	GFile *inodoArchivo;
	ptrGBloque punteroAlInodo = buscarInodoArchivo(path, NORMAL, inodoArchivo);

	if(inodoArchivo){
		// VERIFICAR QUE EL ARCHIVO NO ESTE ABIERTO POR NINGUN PROCESO. SI LO ESTA, LA FUNCION SE CANCELARA
		if(!estaAbierto(punteroAlInodo)){
			// LIBERAR LOS BLOQUES DE DATOS QUE TIENE ASIGNADO
			liberarBloquesAsignados(inodoArchivo->blocks);

			// BORRAR SU ENTRADA DEL DIRECTORIO
			borrarEntrada(inodoArchivo->father_block, punteroAlInodo);

			// LIBERAR SU INODO
			inodoArchivo->state = BORRADO;

			return 0;
		}
	}

	return -1;
}



//////// FUNCIONES AUXILIARES ////////

t_list* listarDirectorio(GFile *directorio){
	if(elEstadoDelArchivoEs(directorio, DIRECTORIO)){
		t_list* lista = list_create();
		GPointerBlock *bloqueDePunteros;
		GDirectoryBlock *bloqueDeDirectorio;
		GDirEntry entrada;
		int numeroEntrada = 0;
		int numeroBloqueDePunteros = 0;
		int numeroBloqueDeDatos = 0;
		int BloquesAsignados = (cantidadBloquesAsignados(directorio->blocks));


		do{
			// OBTENER BLOQUE DE PUNTEROS
			bloqueDePunteros = obtenerBloque(directorio->blocks[numeroBloqueDePunteros]);
			do{
				// OBTENER BLOQUE DE DATOS
				bloqueDeDirectorio = obtenerBloque(bloqueDePunteros->blocks[numeroBloqueDeDatos]);
				do{

					// LEER ENTRADAS
					memcpy(entrada, bloqueDeDirectorio->entries[numeroEntrada],sizeof(GDirEntry));
					if(!entradaVacia(entrada)){
						list_add(lista,entrada);
					}
					numeroEntrada ++;

				}while(!entradaVacia(entrada) && numeroEntrada < ENTRADAS_POR_BLOQUE_DE_DIRECTORIO);
				numeroBloqueDeDatos ++;
			}while(!entradaVacia(entrada) && numeroBloqueDeDatos < 1024);
			numeroBloqueDePunteros ++;
		}while(!entradaVacia(entrada) && numeroBloqueDePunteros < BloquesAsignados);
		return lista;
	}else {
		return NULL;
	}
}

/*t_list* listarDirectorio(GFile *directorio){
	if(elEstadoDelArchivoEs(directorio, DIRECTORIO)){
		t_list* lista = list_create();
		GDirEntry entrada;
		GPointerBlock bloqueDePunteros;
		GDirectoryBlock bloqueDeDirectorio;
		int numeroEntrada = 0;
		int numeroBloqueDePunteros = 0;
		int numeroBloqueDeDatos = 0;
		int BloquesAsignados = (cantidadBloquesAsignados(directorio->blocks));


		do{
			// OBTENER BLOQUE DE PUNTEROS
			lseek(diskFD, OFFSET(directorio->blocks[numeroBloqueDePunteros]), SEEK_SET);
			read(diskFD, &bloqueDePunteros, BLOCK_SIZE);
			do{
				// OBTENER BLOQUE DE DATOS
				lseek(diskFD, OFFSET(bloqueDePunteros.blocks[numeroBloqueDeDatos]), SEEK_SET);
				read(diskFD, &bloqueDeDirectorio, BLOCK_SIZE);
				do{
					memcpy(entrada, bloqueDeDirectorio.entries[numeroEntrada], sizeof(GDirEntry));

					// LEER ENTRADAS
					if(!entradaVacia(entrada)){
						list_add(lista,entrada);
					}


				}while(!entradaVacia(entrada) && numeroEntrada < ENTRADAS_POR_BLOQUE_DE_DIRECTORIO);
				numeroBloqueDeDatos ++;
			}while(!entradaVacia(entrada) && numeroBloqueDeDatos < 1024);
			numeroBloqueDePunteros ++;
		}while(!entradaVacia(entrada) && numeroBloqueDePunteros < BloquesAsignados);
		return lista;
	}else {
		return NULL;
	}
}*/

ptrGBloque buscarInodoArchivo(const char *path, int mode, GFile inodoArchivo){
	char** directorios = string_split(path, '/');
	inodoArchivo = directorioRaiz();
	ptrGBloque punteroAlInodo;

	int cantidadDirectorios = cantidadElementosCharAsteriscoAsterisco(directorios);

	if(mode == SIN_EL_ULTIMO){
		cantidadDirectorios --;
	}
	if(mode == SIN_LOS_DOS_ULTIMOS){
		cantidadDirectorios -= 2;
	}
	int directorioActual = 1;

	while(directorioActual < cantidadDirectorios && inodoArchivo != NULL){
		punteroAlInodo = buscarArchivoEnDirectorio(inodoArchivo, directorios[directorioActual]);
		if(directorioActual < (cantidadDirectorios + 1) && inodoArchivo.state != DIRECTORIO){
			inodoArchivo = NULL;
			return 0;
		}
		directorioActual ++;
	}

	return punteroAlInodo;
}

/*GFile buscarInodoDirectorio(const char *path, int mode){
	char** directorios = string_split(path, '/');
	GFile inodoDirectorio = directorioRaiz();

	int cantidadDirectorios = cantidadElementosCharAsteriscoAsterisco(directorios);

	if(mode == SIN_EL_ULTIMO){
		cantidadDirectorios --;
	}
	int directorioActual = 1;

	while(directorioActual < cantidadDirectorios && inodoDirectorio != NULL){
		GFile inodoDirectorio = buscarArchivoEnDirectorio(inodoDirectorio, directorios[directorioActual]);
		directorioActual ++;
	}

	return inodoDirectorio;
}*/ // CREO QUE CON EL QUE ESTA ABAJO NOS TENDRIA QUE SERVIR

/*ptrGBloque punteroDirectorioPadre(char* path){
	GFile directorioAbuelo = buscarInodoArchivo(path, SIN_LOS_DOS_ULTIMOS);

	char** direccion = string_split(path, '/');
	int longitudPath = cantidadElementosCharAsteriscoAsterisco(direccion);

	buscarArchivoEnDirectorio()

}*/

/*GFile buscarInodoArchivo(const char *path, int mode){
	char** directorios = string_split(path, '/');
	GFile inodoDirectorio = directorioRaiz();

	int cantidadDirectorios = cantidadElementosCharAsteriscoAsterisco(directorios);

	if(mode == SIN_EL_ULTIMO){
		cantidadDirectorios --;
	}
	if(mode == SIN_LOS_DOS_ULTIMOS){
		cantidadDirectorios += 2;
	}
	int directorioActual = 1;

	while(directorioActual < cantidadDirectorios && inodoDirectorio != NULL){
		GFile inodoDirectorio = buscarArchivoEnDirectorio(inodoDirectorio, directorios[directorioActual]);
		if(directorioActual < (cantidadDirectorios + 1) && inodoDirectorio.state != DIRECTORIO){
			return NULL;
		}
		directorioActual ++;
	}

	return inodoDirectorio;
}*/ // POR LA NUEVA PRUEBA DE LA IMPLEMENTACION DE buscarArchivo, VOY A USAR ESTA FUNCION MODIFICADA

bool elEstadoDelArchivoEs(GFile *archivo, uint8_t state){
	return archivo->state == state;
}

GFile directorioRaiz(){
	return (myDisk + INODE_TABLE_START);
}

/*GFile buscarArchivoEnDirectorio(GFile *directorio, char* archivo){
	GDirEntry entrada;
	GFile inodo;

	bool esElArchivo(GDirEntry unaEntrada){
		return strcmp(unaEntrada.fname, archivo);
	}

	t_list* listaDeArchivos = listarDirectorio(directorio);

	if(listaDeArchivos != NULL){
		entrada = list_find(listaDeArchivos, (bool)esElArchivo);

		lseek(diskFD, OFFSET(entrada.inode), SEEK_SET);
		read(diskFD, &inodo, BLOCK_SIZE);

		list_destroy(listaDeArchivos);

		return inodo;
	}else {
		return NULL;
	}
}*/ // PROBAMOS CON ESTA IMPLEMENTACION, QUE DEVUELVE EL PUNTERO AL INODO, Y EL INODO LO DEVUELVE EN UNO DE LOS PARAMETROS

ptrGBloque buscarArchivoEnDirectorio(GFile *directorio, char* archivo){
	GDirEntry *entrada;
	ptrGBloque punteroAlInodo;

	bool esElArchivo(GDirEntry unaEntrada){
		return strcmp(unaEntrada.fname, archivo);
	}

	t_list* listaDeArchivos = listarDirectorio(directorio);

	if(listaDeArchivos != NULL){
		entrada = list_find(listaDeArchivos, esElArchivo);
		punteroAlInodo = entrada->inode;

		directorio = obtenerEntrada(punteroAlInodo);

		list_destroy(listaDeArchivos);

		return punteroAlInodo;
	}else {
		directorio = NULL;
		return 0;
	}
}



int cantidadBloquesAsignados(ptrGBloque array[]){
	int i = 0;
	while(array[i] && i<=1000){
		i ++;
	}
	return i;
}

bool entradaVacia(GDirEntry entrada){ // POSIBLEMENTE TENGA QUE CAMBIARLO POR UN *
	return entrada.inode;
}

GDirEntry *buscarEntrada(ptrGBloque directorioPadre, ptrGBloque archivo){
	GDirEntry *entrada;
	ptrGBloque punteroAlInodo;
	GFile *directorio = obtenerBloque(directorioPadre);

	bool esElArchivo(GDirEntry *unaEntrada){
		return strcmp(unaEntrada->inode, archivo);
	}

	t_list* listaDeArchivos = listarDirectorio(directorio);

	if(listaDeArchivos != NULL){
		entrada = list_find(listaDeArchivos, esElArchivo);

		list_destroy(listaDeArchivos);
	}
	return entrada;
}

int obtenerEntrada(directorioPadre){

}

void borrarEntrada(ptrGBloque directorioPadre, ptrGBloque Archivo){
	GDirEntry *entrada = buscarEntrada(directorioPadre, Archivo);
	entrada->inode = 0;
}


int escribirEnBloque(ptrGBloque bloqueDestino, GBlock *contenido){
	if(bloqueDestino < INODE_TABLE_START){ // control para no escribir dentro del bitmap
		memcpy(obtenerBloque(bloqueDestino), contenido, BLOCK_SIZE);
		return 0;
	}
	return 1;
}

GBlock *obtenerBloque(ptrGBloque bloque){
	return myDisk + bloque;
}

bool noTieneHijos(ptrGBloque punteroAInodo){
	bool tieneAlMenosUnHijo = false;
	GFile *inodo;
	ptrGBloque punteroInodo = obtenerBloque(INODE_TABLE_START + 1); // IGNORAMOS EL DIRECTORIO RAIZ YA QUE NO TIENE PADRE :,(
	int contador = 0;

	while( !tieneAlMenosUnHijo && contador < NODE_TABLE_SIZE){
		if((inodo+contador)->father_block == punteroAInodo){
			tieneAlMenosUnHijo = true;
		}
		contador ++;
	}
	return tieneAlMenosUnHijo;
}

void liberarBloquesAsignados(ptrGBloque blocks[]){
	// LIBERAR LOS BLOQUES DE DATOS QUE TIENE ASIGNADO
	GPointerBlock *bloqueDePunteros;
	int numeroBloqueDePunteros = 0;
	int numeroBloqueDeDatos = 0;
	int BloquesAsignados = (cantidadBloquesAsignados(blocks));

	do{
		// OBTENER BLOQUE DE PUNTEROS
		bloqueDePunteros = obtenerBloque(blocks[numeroBloqueDePunteros]);
		do{
			// LIBERAR BLOQUE DE DATOS
			liberarBloque(bloqueDePunteros->blocks[numeroBloqueDeDatos]);
			numeroBloqueDeDatos ++;
		}while( numeroBloqueDeDatos < 1024 && bloqueDePunteros->blocks[numeroBloqueDeDatos]);
		numeroBloqueDePunteros ++;
	}while(numeroBloqueDePunteros < BloquesAsignados);

	// LIBERAR LOS BLOQUES DE PUNTEROS QUE TIENE ASIGNADO
	for(int i; i < BloquesAsignados; i ++){
		liberarBloque(blocks[i]);
	}
}

bool estaAbierto(ptrGBloque punteroAlInodo){
	bool condicion(GlobalFdNode *nodo){
		return nodo->inodePointer == punteroAlInodo && nodo->numero_aperturas > 0;
	}
	return list_any_satisfy(tablaProcesosAbiertosGlobal, condicion);
}


// manejo char**

uint8_t cantidadElementosCharAsteriscoAsterisco(char** array){
	uint8_t size;
	for(size = 0; array[size] != NULL; size++);
	return size;
}

void liberarCharAsteriscoAsterisco(char** array){
	string_iterate_lines(array, free);
	free(array);
	return;
}
