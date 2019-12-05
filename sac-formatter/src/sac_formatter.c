#include "sac_formatter.h"


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
	int diskFD = open(fileName, O_RDWR);
	GBlock* myDisk = mmap(NULL, diskSize, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, diskFD, 0);

	if (shouldFormat){
		printf("Formateando disco %s...\n", fileName);

		printf("\tEscribiendo el Header en la posicion %p\n", myDisk);
		writeHeader(myDisk);

		printf("\tEscribiendo el Bitmap en la posicion %p\n", NEXT_BLOCK(myDisk));
		writeBitmap(myDisk + 1);

		printf("\tEscribiendo la Tabla de Nodos en la posicion %p\n", NEXT_BLOCK(myDisk) + BITMAP_SIZE_IN_BLOCKS);
		writeNodeTable(NEXT_BLOCK(myDisk) + BITMAP_SIZE_IN_BLOCKS);

		/*munmap(myDisk, diskSize);

		char* myDiskBitmap = mmap(NULL, BITMAP_SIZE_IN_BLOCKS*sizeof(GBlock), PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, diskFD, sizeof(GBlock));

		printf("\tEscribiendo el Bitmap en la posicion %p\n", NEXT_BLOCK(myDisk));
		writeBitmap(myDiskBitmap);

		munmap(myDiskBitmap, BITMAP_SIZE_IN_BLOCKS*sizeof(GBlock));*/

		printf("FINALIZADO");
	} else {

		printf("Dumpeando disco %s...\n\n", fileName);
		printf("Contenidos Del Header:\n");
		dumpHeader(myDisk);

		printf("Contenidos Del Bitmap:\n");
		dumpBitmap(NEXT_BLOCK(myDisk));

		printf("Contenidos Del NodeTable:\n");
		dumpNodeTable(NEXT_BLOCK(myDisk) + BITMAP_SIZE_IN_BLOCKS);

	}
}

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

	// TODO SETEAR DE NUEVO A MAX_FILE_NUMBER
	for(int nFile = 0; nFile < 5; nFile++){
		GFile node = nodeTable[nFile];
		memset(auxName, '\0', (MAX_FILENAME_LENGTH + 1) * sizeof(char));
		memcpy(auxName, node.fname, MAX_FILENAME_LENGTH);

		if (node.state > 0){
			printf("Node %u - State: %s\t Name: %s\tSize: %u FatherBlock: %i\n", nFile, nameByCode(node.state), auxName, node.file_size, node.father_block);
			printf("bloques:\n");
			for(int i = 0; i < 10; i++){
				printf("%i\n", node.blocks[i]);
			}
		} else {
			printf("Node %u - State: %s\n", nFile, nameByCode(node.state));
		}
	}
}

void writeBitmap(char* myDiskBitmap){

	int tamanioEnBytes = BITMAP_SIZE_IN_BLOCKS*BLOCK_SIZE;

	t_bitarray *bitarray = bitarray_create_with_mode(myDiskBitmap, tamanioEnBytes, MSB_FIRST);

	int ocupado = BITMAP_SIZE_IN_BLOCKS + NODE_TABLE_SIZE + 1; // SUMO 1 POR EL HEADER

	for(int cont=0; cont < ocupado; cont++){
		bitarray_set_bit(bitarray, cont);
	}

	for(int cont2=ocupado; cont2 < BITMAP_SIZE_IN_BLOCKS * 4096; cont2++){
		bitarray_clean_bit(bitarray, cont2);
	}

	msync(myDiskBitmap, BITMAP_SIZE_IN_BLOCKS * 4096, MS_SYNC); // ES NECESARIO ESTO?
}

void dumpBitmap(GBlock *diskPointer){
	int valor;
	int tamanioEnBytes = BITMAP_SIZE_IN_BLOCKS*BLOCK_SIZE;
	int loQuePrinteare = BITMAP_SIZE_IN_BLOCKS + NODE_TABLE_SIZE + 30; // 1 POR EL HEADER Y 29 PARA QUE IMPRIMA UNOS CEROS
	t_bitarray *bitarray = bitarray_create_with_mode(diskPointer, tamanioEnBytes, MSB_FIRST);

	// TODO SETEAR DE NUEVO EN 0
	for(int i = 131000; i < loQuePrinteare; i++){
		if(bitarray_test_bit(bitarray, i))
			valor = 1;
		else
			valor = 0;
		printf("%i", valor);
	}
	printf("\n");
	return;
}

void writeHeader(GBlock *diskPointer){
	struct sac_header_t* myNewHeader = (struct sac_header_t*) diskPointer;

	memcpy(myNewHeader->sac, "SAC", MAGIC_NUMBER_NAME);
	myNewHeader->bitmap_size = BITMAP_SIZE_IN_BLOCKS;
	myNewHeader->bitmap_start = BITMAP_START_BLOCK;
	myNewHeader->version = 1;
	myNewHeader->estaElDirectorioRaiz = false;
}

void dumpHeader(GBlock *diskPointer){
	struct sac_header_t* myNewHeader = (struct sac_header_t*) diskPointer;

	char* auxName = malloc(MAGIC_NUMBER_NAME + 1);
	memset(auxName, 0, MAGIC_NUMBER_NAME + 1 * sizeof(char));
	memcpy(auxName, myNewHeader->sac, MAGIC_NUMBER_NAME);

	printf("\tMagic Number: %s\n", auxName);
	printf("\tBitmap Size: %u\n", myNewHeader->bitmap_size);
	printf("\tBitmap Start: %u\n", myNewHeader->bitmap_start);
	printf("\tVersion: %u\n", myNewHeader->version);

	free(auxName);
}

size_t getFileSize(char* file){
	FILE* fd = fopen(file, "r");

	fseek(fd, 0L, SEEK_END);
	uint32_t dfSize = ftell(fd);

	fclose(fd);
	return dfSize;
}
