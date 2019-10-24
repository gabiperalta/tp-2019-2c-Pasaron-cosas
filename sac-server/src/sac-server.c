#include "sac-server.h"

int main(int argc, char *argv[]){

	if(argc == 1){
		printf("No has especificado ningun disco\n");
		return 0;
	}
	char* fileName = argv[1];


	cargarBitmap(fileName);
	pthread_mutex_init(&mutexBitmap, NULL);
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
	diskFD = open(diskName, O_RDWR);
	char* myDiskBitarray = mmap(NULL, BITMAP_SIZE_IN_BLOCKS * BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, diskFD, BLOCK_SIZE);  // DECLARE myDisk COMO UNA VARIABLE GLOBAL EN GESTOR DE MEMORIA

	bitmap = bitarray_create_with_mode(myDiskBitarray, BITMAP_SIZE_IN_BLOCKS * BLOCK_SIZE, MSB_FIRST);

	msync(myDiskBitarray, sizeof(bitmap), MS_SYNC);
}
