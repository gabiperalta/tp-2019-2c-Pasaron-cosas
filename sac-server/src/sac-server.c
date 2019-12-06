#include "sac-server.h"

int main(int argc, char *argv[]){
	t_config* archivo_config = config_create(PATH_CONFIG);

	char * linea;

	if(argc == 1){
		printf("No has especificado ningun disco\n");
		return 0;
	}
	char* fileName = argv[1];



	cargarDisco(fileName);
	pthread_mutex_init(&mutexBitmap, NULL);
	pthread_mutex_init(&mutexEscrituraInodeTable, NULL);
	pthread_mutex_init(&mx_tablaGlobal, NULL);

	 // HASTA ACA FUNCIONA TODO BIEN

	// TODO VERIFICAR SI ESTA EL DIRECTORIO RAIZ, Y SI NO LO ESTA, CREARLO
	GHeader* header = (GHeader*) myDisk;
	if(!header->estaElDirectorioRaiz){
		crearDirectorioRaiz();
		header->estaElDirectorioRaiz = true;
	}

	posicionEnArchivo(941);

	PUERTO = config_get_int_value(archivo_config, "PUERTO_ESCUCHA");

	/*int pruebaBitarray1 = bloqueLibre();
	int pruebaBitarray2 = bloqueLibre();
	int pruebaBitarray3 = bloqueLibre();
	int pruebaBitarray4 = bloqueLibre();*/

	config_destroy(archivo_config);

	tablaProcesosAbiertosGlobal = list_create();

	listaDeTablasDeArchivosPorProceso = list_create();

	inicializarServidor();

	system("clear");

	while(1){
		linea = readline(">");
		/*if (linea) {
			add_history(linea);
		}*/
		if(!strncmp(linea, "exit", 4)) {
			free(linea);
			break;
		}

		free(linea);
	}

	return 0;
}

size_t getFileSize(char* file){
	FILE* fd = fopen(file, "r");

	fseek(fd, 0L, SEEK_END);
	uint32_t dfSize = ftell(fd);

	fclose(fd);
	return dfSize;
}

void cargarDisco(char* diskName){
	size_t diskSize = getFileSize(diskName);
	diskFD = open(diskName, O_RDWR);
	myDisk = mmap(NULL, diskSize, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, diskFD, 0);

	miBitarray = obtenerBloque(BITMAP_START_BLOCK);
	bitmap = bitarray_create_with_mode( miBitarray, BITMAP_SIZE_IN_BLOCKS * BLOCK_SIZE, MSB_FIRST);

	msync(myDisk, sizeof(bitmap), MS_SYNC);
}
