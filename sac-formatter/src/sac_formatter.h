#ifndef GESTOR_DE_MEMOTIA_H_
#define GESTOR_DE_MEMOTIA_H_

#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <commons/bitarray.h>
#include <sys/mman.h>


#define BLOCK_SIZE 4096
#define MAGIC_NUMBER_NAME 3
#define MAX_FILENAME_LENGTH 71
#define MAX_FILE_NUMBER 1024
#define BITMAP_START_BLOCK 1
#define BITMAP_SIZE_IN_BLOCKS 131072
#define NODE_TABLE_SIZE 1024  // EN BLOQUES
#define DIR_ENTRY_SIZE
#define INODE_TABLE_START 131073
#define DATA_BLOCKS_START 132097
#define MAX_FILE_SIZE 4294967296
#define NEXT_BLOCK(A) A + 1
#define OFFSET(offset) offset * 4096

typedef uint32_t ptrGBloque;


typedef struct sac_block {
	unsigned char bytes[BLOCK_SIZE];
} GBlock;

typedef struct sac_header_t {
	unsigned char sac[MAGIC_NUMBER_NAME];
	uint32_t version;
	ptrGBloque bitmap_start;
	uint32_t bitmap_size; // EN BLOQUES
	bool estaElDirectorioRaiz;
	unsigned char padding[4080]; // RELLENO
} GHeader;

typedef struct sac_file_t {
	uint8_t state; // 0: borrado, 1: archivo, 2: directorio
	unsigned char fname[MAX_FILENAME_LENGTH];
	ptrGBloque father_block; // Es cero si esta en el directorio raiz
	uint32_t file_size;
	uint64_t creation_date;
	uint64_t modification_date;
	ptrGBloque blocks[1000];
} GFile;

typedef struct sac_block_IS{ // BLOQUE DE PUNTEROS
	ptrGBloque blocks[1024];
}GPointerBlock;

int main(int argc, char **argv);
void writeHeader(GBlock *diskPointer);
void dumpHeader(GBlock *diskPointer);
void writeBitmap(char* myDiskBitmap);
void dumpBitmap(GBlock *diskPointer);
void dumpNodeTable(GBlock *diskPointer);
void writeNodeTable(GBlock *diskPointer);
void dumpPointerBlock(GBlock* bloque);
char* nameByCode(int code);
size_t getFileSize(char* file);


#endif
