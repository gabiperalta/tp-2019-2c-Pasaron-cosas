#include "sac-server.h"

int main(int argc, char *argv[]){

	if(argc == 1){
		printf("No has especificado ningun disco\n");
		return 0;
	}
	char* fileName = argv[1];

	size_t diskSize = getFileSize(fileName);
	int diskFD = open(fileName, O_RDWR);
	myDisk = mmap(NULL, diskSize, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, diskFD, 0);  // DECLARE myDisk COMO UNA VARIABLE GLOBAL EN GESTOR DE MEMORIA
	close(diskFD);

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
