#include "funcionesSac.h"


//////// FUNCIONES PRINCIPALES ////////

// FUNCIONES GENERALES








// FUNCIONES CON DIRECTORIOS

// MKDIR
int crearDirectorio(const char *path, mode_t mode){
	GFile directorioPadre;
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO, directorioPadre);
	char** pathDividida = string_split(path, '/');
	int longitudDePath = cantidadElementosCharAsteriscoAsterisco(pathDividida);
	int numeroInodo = inodoLibre();
	GFile inodo;
	struct timeval tiempo;

	gettimeofday(tiempo, NULL);

	if(directorioPadre == NULL){
		return -1;
	}

	if(tieneEntradaLibre(directorioPadre)){
		if(inodoLibre >= 0){ // DEBERIA SINCRONIZAR ESTO DE MANERA QUE UNA VEZ QUE SEPA QUE TIENE INODO Y ENTRADA DI DIRECTORIO LIBRES, NADIE SE LOS PUEDA QUITAR
			for(int i; i<1000; i++){ // PUEDO ASIGNARLO Y SI NO PUEDO TERMINAR LA OPREACION LO LIBERA
				inodo.blocks[i] = 0;
			}


			inodo.creation_date = tiempo.tv_usec;
			inodo.father_block = punteroAInodoPadre;
			inodo.file_size = 0;
			memcpy(inodo.fname, pathDividida[longitudDePath - 1]);
			inodo.modification_date = tiempo.tv_usec;
			inodo.state = DIRECTORIO;

		}
	}
}

// RMDIR
int eliminarDirectorio(const char *path){

}

// LS
t_list* listarDirectorio(GFile *directorio){
	if(elEstadoDelArchivoEs(directorio, DIRECTORIO)){
		t_list* lista = list_create();
		GDirEntry entrada;
		GBlockIs bloqueDePunteros;
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
}


// FUNCIONES ARCHIVOS
// MKNOD
int crearArchivo(const char *path, mode_t, dev_t){
	GFile directorioPadre;
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO, directorioPadre);
	char** pathDividida = string_split(path, '/');
	int longitudDePath = cantidadElementosCharAsteriscoAsterisco(pathDividida);
	int numeroInodo = reservarInodo(ARCHIVO);
	GFile inodo;
	struct timeval tiempo;

	gettimeofday(tiempo, NULL);

	if(directorioPadre == NULL){
		return -1;
	}

	if(tieneEntradaLibre(directorioPadre)){
		if(inodoLibre > 0){ // DEBERIA SINCRONIZAR ESTO DE MANERA QUE UNA VEZ QUE SEPA QUE TIENE INODO Y ENTRADA DI DIRECTORIO LIBRES, NADIE SE LOS PUEDA QUITAR
			for(int i; i<1000; i++){ // PUEDO ASIGNARLO Y SI NO PUEDO TERMINAR LA OPREACION LO LIBERA
				inodo.blocks[i] = 0;
			}


			inodo.creation_date = tiempo.tv_usec;
			inodo.father_block = punteroAInodoPadre;
			inodo.file_size = 0;
			memcpy(inodo.fname, pathDividida[longitudDePath - 1]);
			inodo.modification_date = tiempo.tv_usec;
			inodo.state = DIRECTORIO;

		}
	}
}

// OPEN
int abrirArchivo(const char *path, struct fuse_file_info *){
	// REVISAR LA TABLA DE ARCHIVOS ABIERTOS A VER SI EL ARCHIVO YA LO ESTA

	// SI LO ESTA, DIRECTAMENTE SE PASA LA POSICION DE SU FD DENTRO DE LA LISTA

	// SINO, SE CARGA EN LA LISTA, Y RECIEN AHI SE PASA LA POSICION
}


// UNLINK
int eliminarArchivo(const char *path){

	GFile inodoArchivo;
	// BUSCAR INODO DEL ARCHIVO
	ptrGBloque punteroAlInodo = buscarInodoArchivo(path, NORMAL, inodoArchivo);

	if(inodoArchivo){
		// VERIFICAR QUE EL ARCHIVO NO ESTE ABIERTO POR NINGUN PROCESO. (SI LO ESTA, ESTE PROCESO SE BLOQUEARA. PARA ELLO USAREMOS UN SEMAFORO
		// POR ARCHIVO ABIERTO, QUE CONTENGA LA CANTIDAD DE PROCESOS QUE LO TIENEN ABIERTO)

		// LIBERAR LOS BLOQUES DE DATOS QUE TIENE ASIGNADO
		GBlockIs bloqueDePunteros;
		int numeroBloqueDePunteros = 0;
		int numeroBloqueDeDatos = 0;
		int BloquesAsignados = (cantidadBloquesAsignados(inodoArchivo->blocks));


		do{
			// OBTENER BLOQUE DE PUNTEROS
			lseek(diskFD, OFFSET(inodoArchivo->blocks[numeroBloqueDePunteros]), SEEK_SET);
			read(diskFD, &bloqueDePunteros, BLOCK_SIZE);
			do{
				// LIBERAR BLOQUE DE DATOS
				liberarBloque(bloqueDePunteros.blocks[numeroBloqueDeDatos]);
				numeroBloqueDeDatos ++;
			}while( numeroBloqueDeDatos < 1024 && bloqueDePunteros.blocks[numeroBloqueDeDatos]);
			numeroBloqueDePunteros ++;
		}while(numeroBloqueDePunteros < BloquesAsignados);

		// LIBERAR LOS BLOQUES DE PUNTEROS QUE TIENE ASIGNADO
		for(int i; i < BloquesAsignados; i ++){
			liberarBloque(inodoArchivo.blocks[i]);
		}

		// BORRAR SU ENTRADA DEL DIRECTORIO

		// LIBERAR SU INODO

		liberarBloque(punteroAlInodo);
	}
}

// LSEEK
int reposicionearOffsetDeArchivo(int filedes, off_t offset, int whence){
	// VERIFICAR QUE EL FD EXISTA

	// MODIFICAR EL OFFSET DEL FD
}





//////// FUNCIONES AUXILIARES ////////

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

bool elEstadoDelArchivoEs(GFile *archivo, uint8_t state){
	return archivo->state == state;
}

GFile directorioRaiz(){
	GFile directorioRaiz;
	lseek(diskFD, OFFSET(INODE_TABLE_START), SEEK_SET);
	read(diskFD, &directorioRaiz, BLOCK_SIZE);
	return directorioRaiz;
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
	GDirEntry entrada;
	int punteroAlInodo;

	bool esElArchivo(GDirEntry unaEntrada){
		return strcmp(unaEntrada.fname, archivo);
	}

	t_list* listaDeArchivos = listarDirectorio(directorio);

	if(listaDeArchivos != NULL){
		entrada = list_find(listaDeArchivos, (bool)esElArchivo);
		punteroAlInodo = entrada.inode;

		lseek(diskFD, OFFSET(entrada.inode), SEEK_SET);
		read(diskFD, &directorio, BLOCK_SIZE);

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

bool entradaVacia(GDirEntry entrada){
	return entrada.inode;
}


uint8_t cantidadElementosCharAsteriscoAsterisco(char** array){
	uint8_t size;
	for(size = 0; array[size] != NULL; size++);
	return size;
}
