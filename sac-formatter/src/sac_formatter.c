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

	uint32_t tamanioBitmap = (diskSize / 4096) / (4096 * 8);
	if(( diskSize / 4096 ) % (4096 * 8) > 0){
		tamanioBitmap ++;
	}

	if (shouldFormat){
		printf("Formateando disco %s...\n", fileName);

		printf("\tEscribiendo el Header en la posicion %p\n", myDisk);
		writeHeader(myDisk, tamanioBitmap);

		printf("\tEscribiendo el Bitmap en la posicion %p\n", NEXT_BLOCK(myDisk));
		writeBitmap(myDisk + 1, tamanioBitmap);

		printf("\tEscribiendo la Tabla de Nodos en la posicion %p\n", NEXT_BLOCK(myDisk) + BITMAP_SIZE_IN_BLOCKS);
		writeNodeTable(NEXT_BLOCK(myDisk) + tamanioBitmap);

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
		dumpBitmap(NEXT_BLOCK(myDisk), tamanioBitmap);

		printf("Contenidos Del NodeTable:\n");
		dumpNodeTable(NEXT_BLOCK(myDisk) + tamanioBitmap);

		//printf("Dumpeando bloque %u\n", 133123);
		//dumpPointerBlock(myDisk + 133123);

	}
}

void writeHeader(GBlock *diskPointer, uint32_t tamanioBitmap){
	struct sac_header_t* myNewHeader = (struct sac_header_t*) diskPointer;

	memcpy(myNewHeader->sac, "SAC", MAGIC_NUMBER_NAME);
	myNewHeader->bitmap_size = tamanioBitmap;
	myNewHeader->bitmap_start = BITMAP_START_BLOCK;
	myNewHeader->version = 1;
	myNewHeader->estaElDirectorioRaiz = false;
}

void writeBitmap(char* myDiskBitmap, uint32_t tamanioBitmap){

	int tamanioEnBytes = tamanioBitmap * BLOCK_SIZE;

	t_bitarray *bitarray = bitarray_create_with_mode(myDiskBitmap, tamanioEnBytes , MSB_FIRST);

	int ocupado = tamanioBitmap + NODE_TABLE_SIZE + 1; // SUMO 1 POR EL HEADER

	for(int cont=0; cont < ocupado; cont++){
		bitarray_set_bit(bitarray, cont);
	}

	for(int cont2=ocupado ; cont2 < tamanioEnBytes * 8; cont2++){
		bitarray_clean_bit(bitarray, cont2);
	}

	msync(myDiskBitmap, BITMAP_SIZE_IN_BLOCKS * 4096, MS_SYNC); // ES NECESARIO ESTO?
}

void writeNodeTable(GBlock *diskPointer){
	struct sac_file_t *nodeTable = (struct sac_file_t*) diskPointer;

	for (int nFile = 0; nFile < MAX_FILE_NUMBER; nFile++){
		nodeTable[nFile].state = 0;
		memset(nodeTable[nFile].fname, '\0', MAX_FILENAME_LENGTH );
		nodeTable[nFile].father_block = 0;
	}
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

void dumpBitmap(GBlock *diskPointer, uint32_t tamanioBitmap){
	int valor;
	int tamanioEnBytes = tamanioBitmap * BLOCK_SIZE;
	int loQuePrinteare = tamanioBitmap + NODE_TABLE_SIZE + 30; // 1 POR EL HEADER Y 29 PARA QUE IMPRIMA UNOS CEROS
	t_bitarray *bitarray = bitarray_create_with_mode(diskPointer, tamanioEnBytes, MSB_FIRST);

	// TODO SETEAR DE NUEVO EN 0
	for(int i = 0 ; i < loQuePrinteare; i++){
		if(bitarray_test_bit(bitarray, i))
			valor = 1;
		else
			valor = 0;
		printf("%i", valor);
	}
	printf("\n");
	return;
}

void dumpNodeTable(GBlock *diskPointer){
	struct sac_file_t *nodeTable = (struct sac_file_t*) diskPointer;

	char* auxName = malloc(MAX_FILENAME_LENGTH + 1);

	// TODO SETEAR DE NUEVO A MAX_FILE_NUMBER
	for(int nFile = 0; nFile < 15; nFile++){
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


void dumpPointerBlock(GBlock* bloque){
	GPointerBlock* bloqueAuxiliar = (GPointerBlock*) bloque;
	for(int i = 0; i < 1024; i ++){
		printf("%u\t", bloqueAuxiliar->blocks[i]);
	}
	printf("\n");
}

size_t getFileSize(char* file){
	FILE* fd = fopen(file, "r");

	fseek(fd, 0L, SEEK_END);
	uint32_t dfSize = ftell(fd);

	printf("TAMANIO DISCO: %u\n", dfSize/4096);

	fclose(fd);
	return dfSize;
}

char* nameByCode(int code){
	if(code == 0){return "Free";}
	else if(code == 1){ return "File"; }
	else {return "Directory";}
}
