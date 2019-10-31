#include "funcionesSac.h"


//////// FUNCIONES PRINCIPALES ////////

// FUNCIONES GENERALES

//GETATTR
int myGetattr( const char *path, struct stat *statRetorno ){
	GFile *inodoArchivo;
	ptrGBloque punteroInodo;

	punteroInodo = buscarInodoArchivo(path, NORMAL, inodoArchivo);
	if( punteroInodo ){
		statRetorno->st_ino = punteroInodo - INODE_TABLE_START; // esto daria el numero de inodo
		statRetorno->st_mode = inodoArchivo->state | 0777 ; // NO SE COMO SE USA
		statRetorno->st_size = inodoArchivo->file_size;
		statRetorno->st_blksize = BLOCK_SIZE;
		statRetorno->st_blocks = cantidadBloquesAsignados(inodoArchivo->blocks);
		statRetorno->st_mtim.tv_nsec = inodoArchivo->modification_date;
		//statRetorno->st_atim
		//statRetorno->st_ctim =

		return 0;
	}

	return -1;
}






// FUNCIONES CON DIRECTORIOS

// MKDIR
int crearDirectorio(const char *path, mode_t mode){ // mode ni lo usamos
	GFile *directorioPadre;
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO, directorioPadre);
	char** pathDividida = string_split(path, '/');
	int longitudDePath = cantidadElementosCharAsteriscoAsterisco(pathDividida);
	ptrGBloque punteroAInodo = reservarInodo(DIRECTORIO);
	GFile *inodo;
	struct timeval tiempo;
	int hayEntradaLibre;
	GDirectoryBlock* bloqueDeDirectorio;



	gettimeofday(tiempo, NULL);

	if(directorioPadre == NULL){
		return -1;
	}


	if(punteroAInodo > 0){ // DEBERIA SINCRONIZAR ESTO DE MANERA QUE UNA VEZ QUE SEPA QUE TIENE INODO Y ENTRADA DI DIRECTORIO LIBRES, NADIE SE LOS PUEDA QUITAR
		inodo = (GFile*) obtenerBloque(punteroAInodo); // PUEDO ASIGNARLO Y SI NO PUEDO TERMINAR LA OPREACION LO LIBERA
		hayEntradaLibre = reservarEntrada(punteroAInodoPadre, punteroAInodo, pathDividida[longitudDePath - 1]);
		if(hayEntradaLibre){ // retorna true si se reservo correctamente, y false si no habian entradas.
			for(int i; i<1000; i++){
				inodo->blocks[i] = 0;
			}


			inodo->creation_date = tiempo.tv_usec;
			inodo->father_block = punteroAInodoPadre;
			inodo->file_size = 0;
			memcpy(inodo->fname, pathDividida[longitudDePath - 1], MAX_FILENAME_LENGTH);
			inodo->modification_date = tiempo.tv_usec;
			inodo->state = DIRECTORIO;

			bloqueDeDirectorio = asignarBloqueDeDirectorio(inodo);

			// CREAR LAS ENTRADAS . Y ..
			inicializarPrimerasEntradas(bloqueDeDirectorio, punteroAInodo, punteroAInodoPadre);

			liberarCharAsteriscoAsterisco(pathDividida);

			return 0;
		}
		inodo->state = BORRADO; // EN CASO DE QUE NO HAYA UNA ENTRADA DISPONILBE, LIBERA EL INODO QUE LE FUE ASIGNADO PREVIAMENTE
	}
	return -1;
}

// RMDIR
int eliminarDirectorio(const char *path){
	GFile *directorio;
	GFile *directorioPadre;
	ptrGBloque punteroAInodo = buscarInodoArchivo(path, NORMAL, directorio);
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO, directorioPadre);

	if(punteroAInodo != 0){
		if(noTieneHijos(punteroAInodo)){

			liberarBloquesAsignados(directorio->blocks);
			directorio->state = BORRADO;

			borrarEntrada(punteroAInodoPadre,punteroAInodo);

			return 0;

		}
	}

	return -1;
}

// READDIR (LS)
int myReaddir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ){
	GFile *directorio;
	ptrGBloque punteroInodo = buscarInodoArchivo(path, NORMAL, directorio);
	t_list *listaDeArchivos;
	int archivosEnDirectorio;

	char* nombre(GDirEntry *entrada){
		return entrada->fname;
	}


	// TODO SI NO EXISTE EL DIRECTORIO, DEBERIA TIRAR ERROR
	if(punteroInodo != 0){

		listaDeArchivos = listarDirectorio(directorio);

		t_list *listaDeNombres = list_map(listaDeArchivos, nombre);

		archivosEnDirectorio = list_size(listaDeNombres);

		for(int i=0; i<archivosEnDirectorio; i++){
			filler( buffer, list_get(archivosEnDirectorio, i), NULL, 0); // SI QUEREMOS PONER MAS DATOS, DEBEMOS REEMPLAZAR NULL POR UN STAT
		}
		return 0;
	}

	return -1;

}


// FUNCIONES ARCHIVOS
// MKNOD
int crearArchivo(const char *path, mode_t modo, dev_t dev){ // no usamos ni mode ni dev
	GFile *directorioPadre;
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO, directorioPadre);
	char** pathDividida = string_split(path, '/');
	int longitudDePath = cantidadElementosCharAsteriscoAsterisco(pathDividida);
	ptrGBloque numeroInodo = reservarInodo(ARCHIVO);
	GFile *inodo;
	struct timeval tiempo;
	int hayEntradaLibre;

	gettimeofday(tiempo, NULL);

	if(directorioPadre == NULL){
		return -1;
	}


	if(inodoLibre > 0){ // DEBERIA SINCRONIZAR ESTO DE MANERA QUE UNA VEZ QUE SEPA QUE TIENE INODO Y ENTRADA DI DIRECTORIO LIBRES, NADIE SE LOS PUEDA QUITAR
		inodo = (GFile*) obtenerBloque(numeroInodo);	// PUEDO ASIGNARLO Y SI NO PUEDO TERMINAR LA OPREACION LO LIBERA
		hayEntradaLibre = reservarEntrada(punteroAInodoPadre, numeroInodo, pathDividida[longitudDePath - 1]);
		if(hayEntradaLibre){  // retorna true si se reservo correctamente, y false si no habian entradas.
			for(int i; i<1000; i++){
				inodo->blocks[i] = 0;
			}


			inodo->creation_date = tiempo.tv_usec;
			inodo->father_block = punteroAInodoPadre;
			inodo->file_size = 0;
			memcpy(inodo->fname, pathDividida[longitudDePath - 1], MAX_FILENAME_LENGTH);
			inodo->modification_date = tiempo.tv_usec;
			inodo->state = DIRECTORIO;

			liberarCharAsteriscoAsterisco(pathDividida);

			return 0;
		}
		inodo->state = BORRADO; // EN CASO DE QUE NO HAYA UNA ENTRADA DISPONILBE, LIBERA EL INODO QUE LE FUE ASIGNADO PREVIAMENTE
	}
	return -1;
}

// OPEN
int abrirArchivo(const char *path, struct fuse_file_info * info){ // debemos ver si hay que crear el archivo si no existe
	GlobalFdNode *fdNode;
	char** direccion = string_split(path, '/');
	int longitudDireccion = cantidadElementosCharAsteriscoAsterisco(direccion);
	// REVISAR LA TABLA DE ARCHIVOS ABIERTOS A VER SI EL ARCHIVO YA LO ESTA
	bool buscador(GlobalFdNode* nodo){
		return strcmp(direccion[longitudDireccion - 1 ], nodo->fname);
	}

	// SI NO LO ESTA, SE CARGA EN LA LISTA, Y RECIEN AHI SE PASA LA POSICION
	if(!list_any_satisfy(tablaProcesosAbiertosGlobal, buscador)){
		GFile *inodoArchivo;
		ptrGBloque punteroArchivo;

		punteroArchivo = buscarInodoArchivo(path, NORMAL, inodoArchivo);
		if(punteroArchivo){
			fdNode = malloc(sizeof(GlobalFdNode));

			memcpy(fdNode->fname, direccion[longitudDireccion - 1 ], MAX_FILENAME_LENGTH);
			fdNode->inodePointer = punteroArchivo;
			fdNode->numero_aperturas = 0;

			list_add(tablaProcesosAbiertosGlobal, fdNode);
		}
		else{
			return -1;
		}
	}
	else{
		// SI LO ESTA, DIRECTAMENTE SE PASA LA POSICION DE SU FD DENTRO DE LA LISTA
		fdNode = list_find(tablaProcesosAbiertosGlobal, buscador);
	}

	int fileDescriptor = agregarAListaDeArchivosDelProceso(fdNode); // todo implementar

	fdNode->numero_aperturas ++;

	return fileDescriptor;

}

int leerArchivo( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi ){
	int retorno = 0;


	/** Read data from an open file
	 *
	 * Read should return exactly the number of bytes requested except
	 * on EOF or error, otherwise the rest of the data will be
	 * substituted with zeroes.	 An exception to this is when the
	 * 'direct_io' mount option is specified, in which case the return
	 * value of the read system call will reflect the return value of
	 * this operation.
	 */


}

// CLOSE
int cerrarArchivo(const char *path){ // TODAVIA NO SE QUE PARAMETROS LLEVA
	// VERIFICAR QUE EL ARCHIVO ESTE ABIERTO POR ESTE PROCESO

	// SI LO ESTA, SE DISMINUYE EL VALOR DE aperturas DE LA TABLA GLOBAL, Y SE ELIMINA EL ELEMENTO DE LA LISTA DEL PROCESO.
	// SI aperturas ES IGUAL A 0, SE SACA DE LA LISTA GLOBAL DICHO FD

	// SI NO ESTA ABIERTO EL ARCHIVO POR EL PROCESO, DEVUELVE -1
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
		//GDirEntry entrada;
		int numeroEntrada = 0;
		int numeroBloqueDePunteros = 0;
		int numeroBloqueDeDatos = 0;
		int BloquesAsignados = (cantidadBloquesAsignados(directorio->blocks));


		do{
			// OBTENER BLOQUE DE PUNTEROS
			bloqueDePunteros = (GPointerBlock*) obtenerBloque(directorio->blocks[numeroBloqueDePunteros]);
			do{
				// OBTENER BLOQUE DE DATOS
				bloqueDeDirectorio = (GDirectoryBlock*) obtenerBloque(bloqueDePunteros->blocks[numeroBloqueDeDatos]);
				do{

					// LEER ENTRADAS
					/*memcpy(entrada, bloqueDeDirectorio->entries[numeroEntrada],sizeof(GDirEntry));
					if(!entradaVacia(entrada)){
						list_add(lista,entrada);
					}*/
					// CREO QUE ES MEJOR ASI
					if(!entradaVacia(bloqueDeDirectorio->entries[numeroEntrada])){
						list_add(lista, bloqueDeDirectorio->entries[numeroEntrada]);
					}

					numeroEntrada ++;

				}while( numeroEntrada < ENTRADAS_POR_BLOQUE_DE_DIRECTORIO);
				numeroBloqueDeDatos ++;
			}while( numeroBloqueDeDatos < 1024);
			numeroBloqueDePunteros ++;
		}while( numeroBloqueDePunteros < BloquesAsignados);
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

ptrGBloque buscarInodoArchivo(const char *path, int mode, GFile *inodoArchivo){
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

	while((directorioActual < cantidadDirectorios) && (inodoArchivo != NULL)){
		punteroAlInodo = buscarArchivoEnDirectorio(inodoArchivo, directorios[directorioActual]);
		if(directorioActual < (cantidadDirectorios + 1) && inodoArchivo->state != DIRECTORIO){
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

GFile *directorioRaiz(){
	return (GFile*) (myDisk + INODE_TABLE_START);
}

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

		directorio = (GFile*) obtenerBloque(punteroAlInodo);

		list_destroy(listaDeArchivos);

		return punteroAlInodo;
	}else {
		directorio = NULL;
		return 0;
	}
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
	GFile *directorio = (GFile*) obtenerBloque(directorioPadre);

	bool esElArchivo(GDirEntry *unaEntrada){
		return strcmp(unaEntrada->inode, archivo);
	}
	// TENGO QUE VER UNA FORMA DE QUE SI ESTOY BUSCANDO UNA ENTRADA VACIA, Y NECESITO UN NUEVO BLOQUE, SE LO ASIGNE

	t_list* listaDeArchivos = listarDirectorio(directorio);

	if(listaDeArchivos != NULL){
		entrada = list_find(listaDeArchivos, esElArchivo);

		list_destroy(listaDeArchivos); // TODO CREO QUE DES ESTA MANERA ELIMINO LA LISTA, PERO ENTRADA SIGUE EXISTIENDO
										// PORQUE APUNTA DIRECTAMENTE A LA DIRECCION DEL MMAP
		if(archivo == 0 && entrada == NULL){
			asignarBloqueDePunteros(directorio);
		}
	}
	return entrada;
}

int reservarEntrada(ptrGBloque directorioPadre, ptrGBloque punteroAInodo, char* nombreArchivo){
	GDirEntry *entrada= buscarEntrada(directorioPadre, punteroAInodo);
	if(entrada){
		entrada->inode = punteroAInodo;
		memcpy(entrada->fname, nombreArchivo, MAX_FILENAME_LENGTH);
		entrada->file_size = 0;

		return 0;
	}

	return -1;
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
	return (myDisk + bloque);
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
		bloqueDePunteros = (GPointerBlock*) obtenerBloque(blocks[numeroBloqueDePunteros]);
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

int agregarAListaDeArchivosDelProceso(fdNode){ // TODO
	int fileDescriptor;

	list_add(listaDeProcesosAbiertos, fdNode);
	fileDescriptor = list_size(listaDeProcesosAbiertos);

	return fileDescriptor;
}

// RETORNO: EL BLOQUE DE DIRECTORIO EN CASO EXITOSO. NULL EN CASO DE FALLO
GDirectoryBlock *asignarBloqueDeDirectorio(GFile* directorio){
	GPointerBlock *bloqueDePunteros;
	GDirectoryBlock *bloqueDeDirectorio;
	int numeroBloqueDeDatos = 0;
	int ultimoBloqueDePunteros = cantidadBloquesAsignados(directorio->blocks) - 1;

	if(directorio->file_size <= MAX_FILE_SIZE){
		// OBTENER ULTIMO BLOQUE DE PUNTEROS
		bloqueDePunteros = (GPointerBlock*) obtenerBloque(directorio->blocks[ultimoBloqueDePunteros]);

		// BUSCAR SI TIENE ALGUNA ENTRADA VACIA (CERO)
		while(bloqueDePunteros->blocks[numeroBloqueDeDatos] != 0 && numeroBloqueDeDatos < 1024){
			numeroBloqueDeDatos ++;
		}

		// SI LA TIENE, A ESA ENTRADA SE LE ASIGNA EL NUEVO BLOQUE DE DIRECTORIO
		if(bloqueDePunteros->blocks[numeroBloqueDeDatos] == 0){
			bloqueDePunteros->blocks[numeroBloqueDeDatos] = bloqueLibre();

			bloqueDeDirectorio = (GDirectoryBlock*) obtenerBloque(bloqueDePunteros->blocks[numeroBloqueDeDatos]);

		}else{ 	// SI NO LA TIENE, DEBO ASIGNARLE UN NUEVO BLOQUE DE PUNTEROS
			ultimoBloqueDePunteros ++;
			directorio->blocks[ultimoBloqueDePunteros] = bloqueLibre();

			bloqueDePunteros = (GPointerBlock*) obtenerBloque(directorio->blocks[ultimoBloqueDePunteros]);

			// Y EN LA PRIMER ENTRADA DE DICHO BLOQUE ASIGNALE UN NUEVO BLOQUE DE DIRECTORIO
			bloqueDePunteros->blocks[0] = bloqueLibre();

			bloqueDeDirectorio = (GDirectoryBlock*) obtenerBloque(bloqueDePunteros->blocks[0]);

		}

		for(int i = 0; i < ENTRADAS_POR_BLOQUE_DE_DIRECTORIO; i++){
			bloqueDeDirectorio->entries[i].inode = 0;
		}


		return bloqueDeDirectorio;
	}
	return NULL;

}

void inicializarPrimerasEntradas(GDirectoryBlock* bloqueDeDirectorio, ptrGBloque punteroSelf, ptrGBloque punteroPadre){
	memcpy(bloqueDeDirectorio->entries[0].fname, ".");
	bloqueDeDirectorio->entries[0].file_size = 0;
	bloqueDeDirectorio->entries[0].inode = punteroSelf;

	memcpy(bloqueDeDirectorio->entries[0].fname, "..");
	bloqueDeDirectorio->entries[0].file_size = 0;
	bloqueDeDirectorio->entries[0].inode = punteroPadre;
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
