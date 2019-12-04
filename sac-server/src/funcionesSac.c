#include "funcionesSac.h"


//////// FUNCIONES PRINCIPALES ////////

void crearDirectorioRaiz(){
	GFile* directorioRaiz = (GFile*) obtenerBloque(INODE_TABLE_START);
	struct timeval tiempo;
	gettimeofday(&tiempo, NULL);
	GDirectoryBlock* bloqueDeDirectorio;


	directorioRaiz->state = DIRECTORIO;
	directorioRaiz->father_block = 0;
	directorioRaiz->file_size = 0;
	memcpy(directorioRaiz->fname, "/\0", 2);
	directorioRaiz->creation_date = tiempo.tv_usec;
	directorioRaiz->modification_date = tiempo.tv_usec;

	bloqueDeDirectorio = asignarBloqueDeDirectorio(directorioRaiz);

	inicializarPrimerasEntradas(bloqueDeDirectorio, INODE_TABLE_START, 0);

	return;
}

// FUNCIONES GENERALES

//GETATTR
int myGetattr( char *path, struct stat *statRetorno ){
	GFile *inodoArchivo;
	ptrGBloque punteroInodo = 0;

	printf("myGetattr \n");
	printf("El path mandado es: %s \n", path);

	punteroInodo = buscarInodoArchivo(path, NORMAL);

	inodoArchivo = (GFile*) obtenerBloque(punteroInodo);


	printf("ARCHIVO: name: %s \t pather: %i\t state: %i\n", inodoArchivo->fname, inodoArchivo->father_block, inodoArchivo->state);


	if( inodoArchivo->state == DIRECTORIO || inodoArchivo->state == ARCHIVO ){
		if(inodoArchivo->state == DIRECTORIO){
			statRetorno->st_mode = S_IFDIR | 0777;
			statRetorno->st_nlink = 2;
		}
		if(inodoArchivo->state == ARCHIVO){
			statRetorno->st_mode = S_IFREG | 0777;
			statRetorno->st_nlink = 1;
			statRetorno->st_size = inodoArchivo->file_size;
		}


		return 0;
	}

	return -1;
}




// FUNCIONES CON DIRECTORIOS

// MKDIR
int crearDirectorio(char *path ){ // mode ni lo usamos
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO);

	GFile *directorioPadre = (GFile*) obtenerBloque(punteroAInodoPadre);

	char** pathDividida = string_split(path, "/");
	int longitudDePath = cantidadElementosCharAsteriscoAsterisco(pathDividida);
	ptrGBloque punteroAInodo = reservarInodo(DIRECTORIO);
	GFile *inodo;
	struct timeval tiempo;
	int hayEntradaLibre;
	GDirectoryBlock* bloqueDeDirectorio;

	printf("crearDirectorio \n");
	printf("El path mandado es: %s \n", path);


	gettimeofday(&tiempo, NULL);

	if(directorioPadre == NULL){
		return -1;
	}
	if(strlen(pathDividida[longitudDePath - 1 ]) > 71){
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
			memcpy(inodo->fname + strlen(pathDividida[longitudDePath - 1]), "\0", 1); // TODO VERIFICAR QUE ESTO FUNCIONE CORRECTAMENTE
			inodo->modification_date = tiempo.tv_usec;
			inodo->state = DIRECTORIO;

			bloqueDeDirectorio = asignarBloqueDeDirectorio(inodo);

			// CREAR LAS ENTRADAS . Y ..
			inicializarPrimerasEntradas(bloqueDeDirectorio, punteroAInodo, punteroAInodoPadre);

			liberarCharAsteriscoAsterisco(pathDividida);

			printf("Directorio creado: name = %s\t father_block = %i\t punteroInodo = %i", inodo->fname, inodo->father_block, punteroAInodo);

			return 0;
		}
		inodo->state = BORRADO; // EN CASO DE QUE NO HAYA UNA ENTRADA DISPONILBE, LIBERA EL INODO QUE LE FUE ASIGNADO PREVIAMENTE
	}
	return -1;
}

// RMDIR
int eliminarDirectorio( char *path){
	ptrGBloque punteroAInodo = buscarInodoArchivo(path, NORMAL);
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO);
	GFile *directorio = (GFile*) obtenerBloque(punteroAInodo);
	GFile *directorioPadre = (GFile*) obtenerBloque(punteroAInodoPadre);

	printf("eliminarDirectorio \n");
	printf("El path mandado es: %s \n", path);

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
int myReaddir( char *path, void *buffer ){
	ptrGBloque punteroInodo = buscarInodoArchivo(path, NORMAL);
	GFile *directorio = (GFile*) obtenerBloque(punteroInodo);
	t_list *listaDeArchivos;
	int archivosEnDirectorio;

	printf("myReaddir \n");
	printf("El path mandado es: %s \n", path);

	char* nombre(GDirEntry *entrada){
		return &entrada->fname;
	}


	// SI NO EXISTE EL DIRECTORIO, TIRA ERROR
	if(punteroInodo != 0){

		int posicion = 0;
		int longitudNombre;

		listaDeArchivos = listarDirectorio(punteroInodo);

		t_list *listaDeNombres = list_map(listaDeArchivos, (void*)nombre);

		archivosEnDirectorio = list_size(listaDeNombres);

		int tamanioDelBuffer = MAX_FILENAME_LENGTH * archivosEnDirectorio + archivosEnDirectorio ;
		buffer = malloc( MAX_FILENAME_LENGTH * archivosEnDirectorio + archivosEnDirectorio); // ESTE TAMANIO ES POR LOS NOMBRES Y LOS ";" QUE LOS SEPARAN
		memset(buffer, 0, tamanioDelBuffer);


		for(int i=0; i<archivosEnDirectorio; i++){
			printf("%s\n", list_get(listaDeNombres, i));
			longitudNombre = strlen(list_get(listaDeNombres, i));
			printf("%i\n", longitudNombre);
			strncpy(buffer + posicion, list_get(listaDeNombres, i), longitudNombre);
			posicion += longitudNombre;
			strncpy(buffer + posicion, ";", 1);
			posicion += 1;
			printf("BUFFER: %s\n", buffer);
		}

		printf("BUFFER: %s\n", buffer);

		list_destroy_and_destroy_elements(listaDeNombres, (void*)free);

		return 0;
	}

	return -1;

}


// FUNCIONES ARCHIVOS
// MKNOD
int crearArchivo( char *path ){
	ptrGBloque punteroAInodoPadre = buscarInodoArchivo(path, SIN_EL_ULTIMO);

	GFile *directorioPadre = (GFile*) obtenerBloque(punteroAInodoPadre);

	char** pathDividida = string_split(path, "/");
	int longitudDePath = cantidadElementosCharAsteriscoAsterisco(pathDividida);
	ptrGBloque numeroInodo = reservarInodo(ARCHIVO);
	GFile *inodo;
	struct timeval tiempo;
	int hayEntradaLibre;

	gettimeofday(&tiempo, NULL);

	if(directorioPadre == NULL){
		return -1;
	}
	if(strlen(pathDividida[longitudDePath - 1 ]) > 71){
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
			memcpy(inodo->fname + strlen(pathDividida[longitudDePath - 1]), "\0", 1); // TODO VERIFICAR QUE ESTO FUNCIONE CORRECTAMENTE
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
uint8_t abrirArchivo( char *path, int socketProceso){ // debemos ver si hay que crear el archivo si no existe
	GlobalFdNode *fdNode;
	char** direccion = string_split(path, "/");
	int longitudDireccion = cantidadElementosCharAsteriscoAsterisco(direccion);
	// REVISAR LA TABLA DE ARCHIVOS ABIERTOS A VER SI EL ARCHIVO YA LO ESTA
	bool buscador(GlobalFdNode* nodo){ // TODO PROBAR, SI GENERA ALGUN PROBLEMA PORQUE HAY OTRO ARCHIVO CON EL MISMO NOMBRE, BUSCAR EL INODO DEL ARCHIVO Y COMPARAR POR EL PUNTERO DEL INODO
		return !strcmp(direccion[longitudDireccion - 1 ], &nodo->fname);
	}

	// SI NO LO ESTA, SE CARGA EN LA LISTA, Y RECIEN AHI SE PASA LA POSICION
	if(!list_any_satisfy(tablaProcesosAbiertosGlobal, (void*)buscador)){
		pthread_mutex_lock(&mx_tablaGlobal);

		ptrGBloque punteroArchivo = buscarInodoArchivo(path, NORMAL);

		GFile *inodoArchivo = (GFile*) obtenerBloque(punteroArchivo);

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
		fdNode = list_find(tablaProcesosAbiertosGlobal, (void*)buscador);
	}

	uint8_t fileDescriptor = agregarAListaDeArchivosDelProceso(fdNode, socketProceso);


	fdNode->numero_aperturas ++;

	pthread_mutex_unlock(&mx_tablaGlobal);

	return fileDescriptor;

}

int escribirArchivo( char *path, char *buffer, size_t size, off_t offset ){

	uint32_t hastaDondeEscribo;
	FileOffset *offsetInicial;
	FileOffset *offsetFinal;

	// BUSCAR EL INODO DEL ARCHIVO
	ptrGBloque punteroArchivo = buscarInodoArchivo(path, NORMAL);

	GFile *inodoArchivo = (GFile*) obtenerBloque(punteroArchivo);
	// VERIFICAR QUE EL OFFSET SEA MENOR AL TAMANIO MAXIMO DE UN ARCHIVO
	if(MAX_FILE_SIZE >= offset){
		offsetInicial = malloc(sizeof(FileOffset));
		offsetFinal = malloc(sizeof(FileOffset));

		hastaDondeEscribo = minimo(size + offset, MAX_FILE_SIZE); // USAR EL MENOR VALOR ENTRE EL (SIZE + OFFSET) Y EL TAMANIO MAXIMO DE UN ARCHIVO

		posicionEnArchivo( offset, offsetInicial); // USO EL BALOR OBTENIDO Y EL OFFSET PARA DETERMINAR EL PUNTERO INICIAL
		posicionEnArchivo( hastaDondeEscribo, offsetFinal); //Y EL FINAL RESPECTIVAMENTE

		// ASIGNAR TODOS LOS BLOQUES DE DATOS QUE SEAN NECESARIOS PARA REALIZAR LA ESCRITURA
		if(offset+size > inodoArchivo->file_size){
			int bloquesQueNecesita = (offset + size - inodoArchivo->file_size) / BLOCK_SIZE;
			for(int i = 0; i < bloquesQueNecesita; i++){ // TODO, NO SE SI NECESITA SER < O <=

			}

		}

		escribirBloques( inodoArchivo, buffer, offsetInicial, offsetFinal );

		free( offsetFinal );
		free( offsetInicial );

		return hastaDondeEscribo;
	}

	return -1;
}

int leerArchivo( char *path, char *buffer, size_t size, off_t offset ){
	uint32_t loQueEsLeido;
	FileOffset *offsetInicial;
	FileOffset *offsetFinal;

	// BUSCAR EL INODO DEL ARCHIVO
	ptrGBloque punteroArchivo = buscarInodoArchivo(path, NORMAL);
	GFile *inodoArchivo = (GFile*) obtenerBloque(punteroArchivo);
	// VERIFICAR QUE EL OFFSET SEA MENOR QUE EL TAMANIO DEL ARCHIVO
	if(inodoArchivo->file_size >= offset){
		offsetInicial = malloc(sizeof(FileOffset));
		offsetFinal = malloc(sizeof(FileOffset));

		loQueEsLeido = minimo(size + offset, inodoArchivo->file_size); // USAR EL MENOR VALOR ENTRE EL (SIZE + OFFSET) Y EL TAMANIO DEL ARCHIVO
		buffer = malloc( loQueEsLeido ); // MALLOQUEO EL BUFFER CON EL TAMANIO OBTENIDO

		posicionEnArchivo( offset, offsetInicial );// USO EL BALOR OBTENIDO Y EL OFFSET PARA DETERMINAR EL PUNTERO INICIAL
		posicionEnArchivo( loQueEsLeido, offsetFinal ); //Y EL FINAL RESPECTIVAMENTE

		leerBloques( inodoArchivo, buffer, offsetInicial, offsetFinal );

		free( offsetFinal );
		free( offsetInicial );

		return loQueEsLeido;
	}

	return -1;
}

// CLOSE
int cerrarArchivo( char *path, int socketProceso){ // TODAVIA NO SE QUE PARAMETROS LLEVA

	GlobalFdNode *fdNode;
	char** direccion = string_split(path, "/");
	int longitudDireccion = cantidadElementosCharAsteriscoAsterisco(direccion);
	// VERIFICO SI EL ARCHIVO ESTA EN LA TABLA GLOBAL
	bool buscador(GlobalFdNode* nodo){ // TODO PROBAR, SI GENERA ALGUN PROBLEMA PORQUE HAY OTRO ARCHIVO CON EL MISMO NOMBRE, BUSCAR EL INODO DEL ARCHIVO Y COMPARAR POR EL PUNTERO DEL INODO
		return !strcmp(direccion[longitudDireccion - 1 ], &nodo->fname);
	}


	// SI NO LO ESTA, SE CARGA EN LA LISTA, Y RECIEN AHI SE PASA LA POSICION
	if(list_any_satisfy(tablaProcesosAbiertosGlobal, (void*)buscador)){
		fdNode = list_find(tablaProcesosAbiertosGlobal, (void*)buscador);

		// VERIFICAR QUE EL ARCHIVO ESTE ABIERTO POR ESTE PROCESO ( LO HACE LA SIGUIENTE FUNCION )
		int retorno = sacarDeLaListaDeArchivosDelProceso(fdNode, socketProceso);
		return retorno;
	}
	else{
		return -1;
	}
}

// UNLINK
int eliminarArchivo( char *path){

	ptrGBloque punteroAlInodo = buscarInodoArchivo(path, NORMAL);

	GFile *inodoArchivo = (GFile*) obtenerBloque(punteroAlInodo);

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

t_list* listarDirectorio(ptrGBloque punteroDirectorio){
		t_list* lista = list_create();
		int posicionTablaInodos = 0;
		GFile* inodoArchivoTable = directorioRaiz();
		GFile* inodoArchivo = inodoArchivoTable + posicionTablaInodos ;

		// TODO RECORRER LA TABLA DE INODOS Y CARGAR EN UNA ESTRUCTURA CUSTOM, SOLO LA METADATA, SIN LA LISTA DE BLOQUES
		// TODOS LOS INODOS QUE TENGAN EL MISMO NUMERO DE INODO PADRE, SON LOS QUE ESTAN DENTRO DE DICHO DIRECTORIO


		while( posicionTablaInodos < 1023 ){
			if( inodoArchivo->father_block == punteroDirectorio ){
				list_add(lista, string_duplicate(inodoArchivo->fname));
			}
			posicionTablaInodos ++;
			inodoArchivo = inodoArchivoTable + posicionTablaInodos ;
		}



		return lista;
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

ptrGBloque buscarInodoArchivo( char *path, int mode){
	char** directorios = string_split(path, "/");
	GFile* inodoArchivoTable = directorioRaiz();
	ptrGBloque punteroDirAnterior = 0; // PORQUE INICIALMENTE BUSCO EL DIRECTORIO RAIZ, QUE TIENE PADRE=0
	uint16_t posicionTablaInodos = 0; // esto ira de 0 a 1023
	GFile* inodoArchivo = inodoArchivoTable + posicionTablaInodos ;
	char* auxName = malloc(MAX_FILENAME_LENGTH + 1);
	memset(auxName, '\0', (MAX_FILENAME_LENGTH + 1) * sizeof(char));
	memcpy(auxName, inodoArchivo->fname, MAX_FILENAME_LENGTH);

	bool tieneElMismoNombre;

	int cantidadDirectorios = cantidadElementosCharAsteriscoAsterisco(directorios);

	if(mode == SIN_EL_ULTIMO){
		cantidadDirectorios --;
	}
	if(mode == SIN_LOS_DOS_ULTIMOS){
		cantidadDirectorios -= 2;
	}
	int directorioActual = 0;

	bool encontrado = false;

	while((directorioActual < cantidadDirectorios)/* && (inodoArchivo != NULL)*/){

		while( !encontrado && (posicionTablaInodos < 1024) ){

				inodoArchivo = inodoArchivoTable + posicionTablaInodos ;

				memset(auxName, '\0', (MAX_FILENAME_LENGTH + 1) * sizeof(char));
				memcpy(auxName, inodoArchivo->fname, MAX_FILENAME_LENGTH);

				tieneElMismoNombre = !strcmp(directorios[directorioActual], auxName);

				if(tieneElMismoNombre && (inodoArchivo->father_block == punteroDirAnterior)){
					encontrado = true;
				}

				/*if (inodoArchivo->state > 0){
					printf("inodoArchivo %u - State: %i\t Name: %s\tSize: %u\n", posicionTablaInodos, inodoArchivo->state, auxName, inodoArchivo->file_size);
				} else {
					printf("inodoArchivo %u - State: %i\n", posicionTablaInodos, inodoArchivo->state);
				}*/
			posicionTablaInodos ++;
		}
		if(strcmp(directorios[directorioActual], auxName) || inodoArchivo->state == BORRADO){
			return 0;
		}
		punteroDirAnterior = posicionTablaInodos + INODE_TABLE_START;
		posicionTablaInodos = 0;
		directorioActual ++;
	}

	printf("Posicion de inodo: %i\n", posicionTablaInodos + INODE_TABLE_START);

	/*if(inodoArchivo->state == BORRADO){ // TODO, REVISAR QUE NO GENERE PROBLEMAS. LO HICE PARA QUE NO DEVUELVA INODO SI ES UNO BORRADO
		inodoArchivo = NULL;
		return 0;
	}*/

	liberarCharAsteriscoAsterisco(directorios);
	free(auxName);


	return posicionTablaInodos + INODE_TABLE_START;
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
		return !strcmp(&unaEntrada.fname, archivo);
	}

	t_list* listaDeArchivos = listarDirectorio(directorio);

	if(listaDeArchivos != NULL){
		entrada = list_find(listaDeArchivos, (void*)esElArchivo);
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
	return entrada.inode == 0;
}

GDirEntry *buscarEntrada(ptrGBloque directorioPadre, ptrGBloque archivo){
	GDirEntry *entrada;
	GFile *directorio = (GFile*) obtenerBloque(directorioPadre);

	bool esElArchivo(GDirEntry *unaEntrada){
		return unaEntrada->inode == archivo;
	}
	// TENGO QUE VER UNA FORMA DE QUE SI ESTOY BUSCANDO UNA ENTRADA VACIA, Y NECESITO UN NUEVO BLOQUE, SE LO ASIGNE

	t_list* listaDeArchivos = listarDirectorio(directorioPadre);

	if(listaDeArchivos != NULL){
		entrada = list_find(listaDeArchivos, (void*)esElArchivo);

		list_destroy(listaDeArchivos); // TODO CREO QUE DE ESTA MANERA ELIMINO LA LISTA, PERO ENTRADA SIGUE EXISTIENDO
										// PORQUE APUNTA DIRECTAMENTE A LA DIRECCION DEL MMAP
		if(archivo == 0 && entrada == NULL){
			GDirectoryBlock *bloqueDeDirectorioNuevo = asignarBloqueDeDirectorio(directorio);
			entrada = &bloqueDeDirectorioNuevo->entries[0];
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
	inodo = (GFile*) obtenerBloque(INODE_TABLE_START + 1); // IGNORAMOS EL DIRECTORIO RAIZ YA QUE NO TIENE PADRE :,(
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
			liberarBloqueDeDatos(bloqueDePunteros->blocks[numeroBloqueDeDatos]);
			numeroBloqueDeDatos ++;
		}while( numeroBloqueDeDatos < 1024 && bloqueDePunteros->blocks[numeroBloqueDeDatos]);
		numeroBloqueDePunteros ++;
	}while(numeroBloqueDePunteros < BloquesAsignados);

	// LIBERAR LOS BLOQUES DE PUNTEROS QUE TIENE ASIGNADO
	for(int i; i < BloquesAsignados; i ++){
		liberarBloqueDeDatos(blocks[i]);
	}
}

bool estaAbierto(ptrGBloque punteroAlInodo){
	bool condicion(GlobalFdNode *nodo){
		return nodo->inodePointer == punteroAlInodo && nodo->numero_aperturas > 0;
	}
	return list_any_satisfy(tablaProcesosAbiertosGlobal, (void*)condicion);
}

uint8_t agregarAListaDeArchivosDelProceso(GlobalFdNode* fdNode, int socketProceso){

	uint8_t fileDescriptor;
	ProcessFdNode *nuevoNodo;

	bool buscadorDeNodo(ProcessTableNode* unNodo){
		return unNodo->socket == socketProceso;
	}

	bool esElArchivo(ProcessFdNode* unNodo){
		return unNodo->archivo == fdNode->inodePointer;
	}

	ProcessTableNode *nodoProceso = list_find(listaDeTablasDeArchivosPorProceso, (void*)buscadorDeNodo);

	// SI EL ARCHIVO YA ESTA ABIERTO POR ESTE PROCESO, NO SE HACE NADA, Y DEVUELVE EL FD QUE YA TENIA ASIGNADO
	if(!list_any_satisfy(nodoProceso->archivos_abiertos, (void*)esElArchivo)){
		nuevoNodo = malloc(sizeof(ProcessFdNode));

		fileDescriptor = obtenerFD(nodoProceso->archivos_abiertos);

		nuevoNodo->archivo = fdNode->inodePointer;
		nuevoNodo->fd = fileDescriptor;

		list_add(nodoProceso->archivos_abiertos, nuevoNodo);
	}else{
		nuevoNodo = list_find(nodoProceso->archivos_abiertos, (void*)esElArchivo);
	}

	return fileDescriptor;
}

int sacarDeLaListaDeArchivosDelProceso(GlobalFdNode* fdNode, int socketProceso){

	bool buscadorDeNodo(ProcessTableNode* unNodo){
		return unNodo->socket == socketProceso;
	}

	bool esElArchivo(ProcessFdNode* unNodo){
		return unNodo->archivo == fdNode->inodePointer;
	}

	ProcessTableNode *nodoProceso = list_find(listaDeTablasDeArchivosPorProceso, (void*)buscadorDeNodo);
	// VERIFICAR SI EL ARCHIVO ESTA ABIERTO POR EL PORCESO
	if( list_any_satisfy(nodoProceso->archivos_abiertos, (void*)esElArchivo) ){
		// SI LO ESTA, SE DISMINUYE EL VALOR DE aperturas DE LA TABLA GLOBAL, Y SE ELIMINA EL ELEMENTO DE LA LISTA DEL PROCESO.
		fdNode->numero_aperturas --;
		list_remove_and_destroy_by_condition(nodoProceso->archivos_abiertos, (void*)esElArchivo, free);

		pthread_mutex_lock(&mx_tablaGlobal);
		if(!fdNode->numero_aperturas){ 	// SI aperturas ES IGUAL A 0, SE SACA DE LA LISTA GLOBAL DICHO FD
			sacarArchivoDeTablaGlobal(fdNode);
		}
		pthread_mutex_unlock(&mx_tablaGlobal);

		return 0;
	}else { // SI NO ESTA ABIERTO EL ARCHIVO POR EL PROCESO, DEVUELVE -1
		return -1;
	}

}

void sacarArchivoDeTablaGlobal(GlobalFdNode* fdNode){
	bool esElNodo(GlobalFdNode* unNodo){
		return unNodo->inodePointer == fdNode->inodePointer;
	}

	void sacarArchivo(GlobalFdNode* unNodo){
		free(unNodo->fname);
		free(unNodo);
	}

	list_remove_and_destroy_by_condition(tablaProcesosAbiertosGlobal, (void*)esElNodo, (void*)sacarArchivo);
}


uint8_t obtenerFD(t_list* listaDeArchivosAbiertos){
	uint8_t fileDescriptor = 0;

	bool tieneAlFD( ProcessFdNode * nodo){
		return nodo->fd == fileDescriptor;
	}

	if(  list_size(listaDeArchivosAbiertos) ){ // SI LA LISTA ESTA VACIA, DEVUELVE EL 0
		while( list_any_satisfy(listaDeArchivosAbiertos, (void*)tieneAlFD)){
			fileDescriptor ++;
		}
	}

	return fileDescriptor;
}

// RETORNO: EL BLOQUE DE DIRECTORIO EN CASO EXITOSO. NULL EN CASO DE FALLO
GDirectoryBlock *asignarBloqueDeDirectorio(GFile* directorio){
	GPointerBlock *bloqueDePunteros;
	GDirectoryBlock *bloqueDeDirectorio;
	int numeroBloqueDeDatos = 0;
	int ultimoBloqueDePunteros = minimo(cantidadBloquesAsignados(directorio->blocks) - 1, 0);


	if(directorio->file_size <= MAX_FILE_SIZE){

		if(!directorio->blocks[ultimoBloqueDePunteros]){
			directorio->blocks[ultimoBloqueDePunteros] = bloqueLibre();
		}
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

		directorio->file_size += BLOCK_SIZE;

		return bloqueDeDirectorio;
	}
	return NULL;

}

void inicializarPrimerasEntradas(GDirectoryBlock* bloqueDeDirectorio, ptrGBloque punteroSelf, ptrGBloque punteroPadre){
	memcpy(bloqueDeDirectorio->entries[0].fname, ".", 1);
	bloqueDeDirectorio->entries[0].file_size = 0;
	bloqueDeDirectorio->entries[0].inode = punteroSelf;

	memcpy(bloqueDeDirectorio->entries[0].fname, "..", 2);
	bloqueDeDirectorio->entries[0].file_size = 0;
	bloqueDeDirectorio->entries[0].inode = punteroPadre;
}

// FUNCIONES AUXILIARES PARA EL MANEJO DE ARCHIVOS

void posicionEnArchivo(uint32_t offset, FileOffset* offsetDelArchivo){
	int restoAnterior;

	offsetDelArchivo->bloqueDePunteros = offset/CAPACIDAD_DIRECCIONAMIENTO_BLOQUE_DE_PUNTEROS;
	restoAnterior = offset%CAPACIDAD_DIRECCIONAMIENTO_BLOQUE_DE_PUNTEROS;

	offsetDelArchivo->bloqueDeDatos = restoAnterior/BLOCK_SIZE;
	restoAnterior = restoAnterior%BLOCK_SIZE;

	offsetDelArchivo->posicionEnBloqueDeDatos = restoAnterior;
}

void escribirBloques(GFile* inodoArchivo, char* buffer, FileOffset* offsetInicial, FileOffset* offsetFinal){
	GPointerBlock* bloqueDePunteros;
	GBlock* bloqueDeDatos;
	int contadorBloquePunteros = offsetInicial->bloqueDePunteros;
	int contadorBloqueDatos = offsetInicial->bloqueDeDatos;
	int punteroBuffer = 0;

	// ESCRIBO EN EL PRIMER BLOQUE DE DATOS
	bloqueDePunteros = (GPointerBlock*) obtenerBloque(inodoArchivo->blocks[contadorBloquePunteros]);
	bloqueDeDatos = obtenerBloque(bloqueDePunteros->blocks[contadorBloqueDatos]);

	// SI LO QUE QUIERO ESCRIBIR ESTA DENTRO DE UN UNICO BLOQUE. Perdon por el if horrible :(
	if(offsetInicial->bloqueDePunteros == offsetFinal->bloqueDePunteros && offsetInicial->bloqueDeDatos == offsetFinal->bloqueDeDatos){
		memcpy(&bloqueDeDatos->bytes[offsetInicial->posicionEnBloqueDeDatos], buffer, offsetFinal->posicionEnBloqueDeDatos - offsetInicial->posicionEnBloqueDeDatos);
		return;
	}

	memcpy(&bloqueDeDatos->bytes[offsetInicial->posicionEnBloqueDeDatos], buffer, BLOCK_SIZE - offsetInicial->posicionEnBloqueDeDatos);
	punteroBuffer += BLOCK_SIZE - offsetInicial->posicionEnBloqueDeDatos; // TODO, VER QUE PASA SI EL BUFFER ES MAS CHICO QUE ESTO. LO MISMO PARA LEER

	contadorBloqueDatos ++;
	// ESCRIBO EN LOS BLOQUES INTERMEDIOS
	while( contadorBloquePunteros < offsetFinal->bloqueDePunteros ){
		while( contadorBloqueDatos < 1024 ){
			bloqueDeDatos = obtenerBloque( bloqueDePunteros->blocks[contadorBloqueDatos] );
			memcpy(&bloqueDeDatos->bytes, buffer + punteroBuffer, BLOCK_SIZE );
			punteroBuffer += BLOCK_SIZE;
			contadorBloqueDatos ++;
		}
		contadorBloqueDatos = 0;
		contadorBloquePunteros ++;
		bloqueDePunteros = (GPointerBlock*) obtenerBloque(inodoArchivo->blocks[contadorBloquePunteros]);
	}

	// ESCRIBO EN LOS BLOQUES DEL ULTIMO BLOQUE DE PUNTEROS
	while( contadorBloqueDatos < offsetFinal->bloqueDeDatos ){
		bloqueDeDatos = obtenerBloque(bloqueDePunteros->blocks[contadorBloqueDatos]);
		memcpy(&bloqueDeDatos->bytes, buffer + punteroBuffer, BLOCK_SIZE);
		punteroBuffer += BLOCK_SIZE;
		contadorBloqueDatos ++;
	}

	// ESCRIBO EN EL ULTIMO BLOQUE DE DATOS
	bloqueDeDatos = obtenerBloque(bloqueDePunteros->blocks[contadorBloqueDatos]);
	memcpy(&bloqueDeDatos->bytes, buffer + punteroBuffer, offsetFinal->posicionEnBloqueDeDatos);

	return;
}




void leerBloques(GFile* inodoArchivo, char* buffer, FileOffset* offsetInicial, FileOffset* offsetFinal){
	GPointerBlock* bloqueDePunteros;
	GBlock* bloqueDeDatos;
	int contadorBloquePunteros = offsetInicial->bloqueDePunteros;
	int contadorBloqueDatos = offsetInicial->bloqueDeDatos;
	int punteroBuffer = 0;

	// LEO EL PRIMER BLOQUE DE DATOS
	bloqueDePunteros = (GPointerBlock*) obtenerBloque(inodoArchivo->blocks[contadorBloquePunteros]);
	bloqueDeDatos = obtenerBloque(bloqueDePunteros->blocks[contadorBloqueDatos]);

	// SI LO QUE QUIERO LEER ESTA DENTRO DE UN UNICO BLOQUE. Perdon por el if horrible :(
	if(offsetInicial->bloqueDePunteros == offsetFinal->bloqueDePunteros && offsetInicial->bloqueDeDatos == offsetFinal->bloqueDeDatos){
		memcpy(buffer, &bloqueDeDatos->bytes[offsetInicial->posicionEnBloqueDeDatos],  offsetFinal->posicionEnBloqueDeDatos - offsetInicial->posicionEnBloqueDeDatos);
		return;
	}

	memcpy(buffer, &bloqueDeDatos->bytes[offsetInicial->posicionEnBloqueDeDatos], BLOCK_SIZE - offsetInicial->posicionEnBloqueDeDatos);
	punteroBuffer += BLOCK_SIZE - offsetInicial->posicionEnBloqueDeDatos;

	contadorBloqueDatos ++;
	// LEO LOS BLOQUES DE DATOS INTERMEDIOS
	while(contadorBloquePunteros < offsetFinal->bloqueDePunteros){
		while(contadorBloqueDatos < 1024){
			bloqueDeDatos = obtenerBloque(bloqueDePunteros->blocks[contadorBloqueDatos]);
			memcpy(buffer + punteroBuffer, &bloqueDeDatos->bytes, BLOCK_SIZE);
			punteroBuffer += BLOCK_SIZE;
			contadorBloqueDatos ++;
		}
		contadorBloqueDatos = 0;
		contadorBloquePunteros ++;
		bloqueDePunteros = (GPointerBlock*) obtenerBloque(inodoArchivo->blocks[contadorBloquePunteros]);
	}
	// LEO LOS BLOQUES DE DATOS QUE CONTIENE EL BLOQUE DE PUNTEROS FINAL
	while(contadorBloqueDatos < offsetFinal->bloqueDeDatos){
		bloqueDeDatos = obtenerBloque(bloqueDePunteros->blocks[contadorBloqueDatos]);
		memcpy(buffer + punteroBuffer, &bloqueDeDatos->bytes, BLOCK_SIZE);
		punteroBuffer += BLOCK_SIZE;
		contadorBloqueDatos ++;
	}

	// LEO EL BLOQUE DE DATOS FINAL
	bloqueDeDatos = obtenerBloque(bloqueDePunteros->blocks[contadorBloqueDatos]);

	memcpy(buffer + punteroBuffer, bloqueDeDatos->bytes, offsetFinal->posicionEnBloqueDeDatos);

	return;
}

GBlock *asignarBloqueDeDatos(GFile* archivo){
	GPointerBlock *bloqueDePunteros;
	GBlock *bloqueDeDatos;
	int numeroBloqueDeDatos = 0;
	int ultimoBloqueDePunteros = minimo(cantidadBloquesAsignados(archivo->blocks) - 1, 0);

	printf("ultimoBloqueDePunteros: %i\n", ultimoBloqueDePunteros);

	if(archivo->file_size <= MAX_FILE_SIZE){

		if(!archivo->blocks[ultimoBloqueDePunteros]){
			archivo->blocks[ultimoBloqueDePunteros] = bloqueLibre();
		}
		// OBTENER ULTIMO BLOQUE DE PUNTEROS
		bloqueDePunteros = (GPointerBlock*) obtenerBloque(archivo->blocks[ultimoBloqueDePunteros]);

		// BUSCAR SI TIENE ALGUNA ENTRADA VACIA (CERO)
		while(bloqueDePunteros->blocks[numeroBloqueDeDatos] != 0 && numeroBloqueDeDatos < 1024){
			numeroBloqueDeDatos ++;
		}

		// SI LA TIENE, A ESA ENTRADA SE LE ASIGNA EL NUEVO BLOQUE DE archivo
		if(bloqueDePunteros->blocks[numeroBloqueDeDatos] == 0){
			bloqueDePunteros->blocks[numeroBloqueDeDatos] = bloqueLibre();

			bloqueDeDatos = (GBlock*) obtenerBloque(bloqueDePunteros->blocks[numeroBloqueDeDatos]);

		}else{ 	// SI NO LA TIENE, DEBO ASIGNARLE UN NUEVO BLOQUE DE PUNTEROS
			ultimoBloqueDePunteros ++;
			archivo->blocks[ultimoBloqueDePunteros] = bloqueLibre();

			bloqueDePunteros = (GPointerBlock*) obtenerBloque(archivo->blocks[ultimoBloqueDePunteros]);

			// Y EN LA PRIMER ENTRADA DE DICHO BLOQUE ASIGNALE UN NUEVO BLOQUE DE archivo
			bloqueDePunteros->blocks[0] = bloqueLibre();

			bloqueDeDatos = (GBlock*) obtenerBloque(bloqueDePunteros->blocks[0]);

		}

		archivo->file_size += BLOCK_SIZE;

		return bloqueDeDatos;
	}
	return NULL;
}


// manejo char**

uint8_t cantidadElementosCharAsteriscoAsterisco(char** array){
	uint8_t size;
	for(size = 0; array[size] != NULL; size++);
	return size;
}

void liberarCharAsteriscoAsterisco(char** array){
	string_iterate_lines(array, (void*)free);
	free(array);
	return;
}

//////////

int maximo(int unNumero, int otroNumero){ // TODO VER SI FUNCIONA CON EL TIPO INT
	if(unNumero <= otroNumero)
		return otroNumero;
	else
		return unNumero;
}

uint32_t minimo(uint32_t unNumero, uint32_t otroNumero){
	if(unNumero <= otroNumero)
		return unNumero;
	else
		return otroNumero;
}

