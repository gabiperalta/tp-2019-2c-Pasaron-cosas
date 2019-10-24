#include "funcionesSac.h"


//////// FUNCIONES PRINCIPALES ////////
// FUNCIONES CON DIRECTORIOS

int crearDirectorio(const char *path, mode_t mode){
	GFile directorioPadre = buscarInodoDirectorio(path, SIN_EL_ULTIMO);
	int numeroInodo = inodoLibre();
	GFile inodo;
	struct timeval tiempo;

	gettimeofday(tiempo, NULL);

	if(directorioPadre == NULL){
		return -1;
	}

	if(tieneEntradaLibre(directorioPadre)){
		if(inodoLibre >= 0){
			for(int i; i<1000; i++){
				inodo.blocks[i] = 0;
			}


			inodo.creation_date = tiempo.tv_usec;
			 //inodo.father_block =  TENGO QUE VER COMO OBTENGO EL INODO PADRE
			inodo.file_size = 0;
			//memcpy(inodo.fname,); // NO ENTIENDO COMO SE USA mode //
			inodo.modification_date = tiempo.tv_usec;
			inodo.state = DIRECTORIO;

		}
	}
}

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
			}while(!entradaVacia(entrada) && numeroBloqueDeDatos < 1024);
			numeroBloqueDePunteros ++;
		}while(!entradaVacia(entrada) && numeroBloqueDePunteros < BloquesAsignados);
		return lista;
	}else {
		return NULL;
	}
}













//////// FUNCIONES AUXILIARES ////////

GFile buscarInodoDirectorio(const char *path, int mode){
	char** directorios = string_split(path, '/');
	GFile inodoDirectorio = directorioRaiz();

	int cantidadDirectorios = cantidadElementosCharAsteriscoAsterisco(directorios);

	if(mode == SIN_EL_ULTIMO){
		cantidadDirectorios --;
	}
	int directorioActual = 1;

	do{
		GFile inodoDirectorio = buscarArchivoEnDirectorio(inodoDirectorio, directorios[directorioActual]);
	}while(directorioActual < cantidadDirectorios && inodoDirectorio != NULL);

	return inodoDirectorio;
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

GFile buscarArchivoEnDirectorio(GFile *directorio, char* archivo){
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
