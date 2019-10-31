#include "sac-server.h"


#define GETATTR 0
#define MKDIR 1
#define RMDIR 2
#define READDIR 3
#define MKNOD 4
#define OPEN 5
#define UNLINK 6
#define CLOSE 7

void gestionarFuncionFilesystem(char* request){

	char** requestSeparada = string_split(request, ' ');
	int funcion = -1;
	struct stat *stat;
	int respuesta = -1;

	if(strcmp(requestSeparada[0], "getattr")){
		funcion = GETATTR;
	}if(strcmp(requestSeparada[0], "mkdir")){
		funcion = MKDIR;
	}if(strcmp(requestSeparada[0], "rmdir")){
		funcion = RMDIR;
	}if(strcmp(requestSeparada[0], "readdir")){
		funcion = READDIR;
	}if(strcmp(requestSeparada[0], "mknod")){
		funcion = MKNOD;
	}if(strcmp(requestSeparada[0], "open")){
		funcion = OPEN;
	}if(strcmp(requestSeparada[0], "unlink")){
		funcion = UNLINK;
	}if(strcmp(requestSeparada[0], "close")){
		funcion = CLOSE;
	}


	switch(funcion){
		case 0://GETATTR

			respuesta = myGetattr(requestSeparada[1], stat);

			if(!respuesta){
				printf("PUNT INODO: %d\n"
						"MODE: %i\n"
						"SIZE: %l"
						"BLOCKSIZE: %d"
						"BLOCKS: %l"
						"MTIM: %l"
						, stat->st_ino, stat->st_mode, stat->st_size, stat->st_blksize, stat->st_blocks, stat->st_mtim);
			}else{
				printf("ERROR");
			}


			break;
		case 1://MKDIR

			respuesta = crearDirectorio(requestSeparada[1]);

			if(!respuesta){
				printf("SE CREO EL DIRECTORIO");
			}else{
				printf("ERROR");
			}


			break;
		case 2://RMDIR

			respuesta = eliminarDirectorio(requestSeparada[1], NULL);

			if(!respuesta){
				printf("SE BORRO EL DIRECTORIO");
			}else{
				printf("ERROR");
			}

			break;
		case 3://READDIR

			break;
		case 4://MKNOD

			respuesta = crearArchivo(requestSeparada[1], NULL, NULL);

			if(!respuesta){
				printf("SE CREO EL ARCHIVO");
			}else{
				printf("ERROR");
			}


			break;
		case 5://OPEN

			respuesta = abrirArchivo(requestSeparada[1], NULL);

			if(!respuesta){
				printf("SE ABRIO EL ARCHIVO");
			}else{
				printf("ERROR");
			}


			break;
		case 6://UNLINK

			respuesta = eliminarArchivo(requestSeparada[1]);

			if(!respuesta){
				printf("SE ELIMINO EL ARCHIVO");
			}else{
				printf("ERROR");
			}


			break;
		case 7://CLOSE


			if(!respuesta){
				printf("SE CERRO EL ARCHIVO");
			}else{
				printf("ERROR");
			}

			break;
	}
}




int main(int argc, char *argv[]) {

	char * linea;

	if(argc == 1){
		printf("No has especificado ningun disco\n");
		return 0;
	}
	char* fileName = argv[1];


	cargarBitmap(fileName);
	pthread_mutex_init(&mutexBitmap, NULL);
	pthread_mutex_init(&mutexEscrituraInodeTable, NULL);

	procesosAbiertosGlobal = list_create();
	inicializarServidor();

	system("clear");

	while(1) {
		linea = readline(">");
		if (linea) {
			add_history(linea);
		}
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}

		gestionarFuncionFilesystem(request);
		free(linea);
	}

	return 0;
}
