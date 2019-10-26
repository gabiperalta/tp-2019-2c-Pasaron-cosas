#include "sac-server.h"

int main(int argc, char *argv[]){

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
	GBlock* myDisk = mmap(NULL, diskSize, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, diskFD, 0);

	bitmap = bitarray_create_with_mode(NEXT_BLOCK(myDisk), BITMAP_SIZE_IN_BLOCKS * BLOCK_SIZE, MSB_FIRST);

	msync(myDisk, sizeof(bitmap), MS_SYNC);
}
