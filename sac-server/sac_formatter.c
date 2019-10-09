#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "sac-server.h"

void writeNodeTable(GBlock *diskPointer){
	struct sac_file_t *nodeTable = (struct sac_file_t*) diskPointer;

	for (int nFile = 0; nFile < MAX_FILE_NUMBER; nFile++){
		nodeTable[nFile].state = 0;
	}
}

char* nameByCode(int code){
	if(code == 0){return "Free";}
	else if(code == 1){ return "File"; }
	else {return "Directory";}
}

void dumpNodeTable(GBlock *diskPointer){
	struct sac_file_t *nodeTable = (struct sac_file_t*) diskPointer;

	char* auxName = malloc(MAX_FILENAME_LENGTH + 1);

	for(int nFile = 0; nFile < MAX_FILE_NUMBER; nFile++){
		GFile node = nodeTable[nFile];
		memset(auxName, '\0', (MAX_FILENAME_LENGTH + 1) * sizeof(char));
		memcpy(auxName, node.fname, MAX_FILENAME_LENGTH);

		if (node.state > 0){
			printf("Node %u - State: %s\t Name: %s\tSize: %u\n", nFile, nameByCode(node.state), auxName, node.fileSize);
		} else {
			printf("Node %u - State: %s\n", nFile, nameByCode(node.state));
		}
	}
}

size_t getFileSize(char* file){
	FILE* fd = fopen(file, "r");

	fseek(fd, 0L, SEEK_END);
	uint32_t dfSize = ftell(fd);

	fclose(fd);
	return dfSize;
}

void writeHeader(GBlock *diskPointer){
	struct sac_header_t* myNewHeader = (struct sac_header_t*) diskPointer;

	memcpy(myNewHeader->sac, "SAC", MAGIC_NUMBER_SIZE);
	myNewHeader->bitmap_size = BITMAP_SIZE_IN_BLOCKS;
	myNewHeader->bitmap_start = BITMAP_START_BLOCK;
	myNewHeader->version = 1;
}

void dumpHeader(GBlock *diskPointer){
	struct sac_header_t* myNewHeader = (struct ufa_header_t*) diskPointer;

	char* auxName = malloc(MAGIC_NUMBER_NAME + 1);
	memset(auxName, 0, MAGIC_NUMBER_NAME + 1 * sizeof(char));
	memcpy(auxName, myNewHeader->sac, MAGIC_NUMBER_NAME);

	printf("\tMagic Number: %s\n", auxName);
	printf("\tBitmap Size: %u\n", myNewHeader->bitmap_size);
	printf("\tBitmap Start: %u\n", myNewHeader->bitmap_start);
	printf("\tVersion: %u\n", myNewHeader->version);

	free(auxName);
}

void writeBitmap(){
	//seguir
}

int main(int argc, char **argv){
	char* fileName;
	bool shouldFormat;
	if (argc == 3 && (strcmp(argv[1], "--format") || (strcmp(argv[1], "-f")))){
		shouldFormat = true;
		fileName = argv[2];
	} else {
		shouldFormat = false;
		fileName = argv[1];
	}


	size_t diskSize = getFileSize(fileName);
	int diskFD = open(fileName, 0_RDWR, 0);
	GBlock* myDisk = mmap(NULL, diskSize, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, diskFD, 0);

	if (shouldFormat){
		printf("Formateando disco %s...\n", fileName);

		printf("\tEscribiendo el Header en la posicion %p\n", myDisk);
		writeHeader(myDisk);

		printf("\tEscribiendo el Bitmap en la posicion %p\n", NEXT_BLOCK(myDisk));
		writeBitmap(NEXT_BLOCK(myDisk));

		printf("\tEscribiendo la Tabla de Nodos en la posicion %p\n", NEXT_BLOCK(myDisk) + BITMAP_SIZE_IN_BLOCKS);
		writeNodeTable(NEXT_BLOCK(myDisk) + BITMAP_SIZE_IN_BLOCKS);

		printf("FINALIZADO");
	} else {

		printf("Dumpeando disco %s...\n\n", fileName);
		printf("Contenidos Del Header:\n");
		dumpHeader(myDisk);



	}
}
