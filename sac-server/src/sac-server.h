#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <fcntl.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <sys/mman.h>
#include <sys/time.h>
#include "servidor.h"
#include "gestorDeMemoria.h"


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
#define NEXT_BLOCK(A) A + 1
#define OFFSET(offset) offset * 4096



#define ENTRADAS_POR_BLOQUE_DE_DIRECTORIO 32



// STATES

#define BORRADO 0
#define ARCHIVO 1
#define DIRECTORIO 2


// TAMANIO BLOQUE * CANTIDAD DE BLOQUES = TAMANIO MAXIMO DE DISCO
//       2^12     *        2^X          =       2^44   =>   X = 32
//		 ( 2^32 ) / 8 = 2^29  =>  ( 2^29 ) / ( 2^12 ) = 131.072	(TAMANIO DEL BITMAP EN BLOQUES)

//////// VARIABLES GLOBALES ////////

//GBlock* directorioRaiz;
t_bitarray *bitmap;
int diskFD;

////////////////////////////////////

typedef uint32_t ptrGBloque;

typedef struct sac_block {
	unsigned char bytes[BLOCK_SIZE];
} GBlock;

typedef struct sac_dir_entry{
	unsigned char fname[MAX_FILENAME_LENGTH];
	uint32_t file_size;
	ptrGBloque inode; // si tiene asignado 0, la entrada esta vacia
	unsigned char padding[49]; // RELLENO
}GDirEntry;

typedef struct sac_directory_block {
	GDirEntry entries[32]; // aca iria la cantidad de entradas que entran en un bloque
}GDirectoryBlock;

typedef struct sac_header_t {
	unsigned char sac[MAGIC_NUMBER_NAME];
	uint32_t version;
	ptrGBloque bitmap_start;
	uint32_t bitmap_size; // EN BLOQUES
	unsigned char padding[4081]; // RELLENO
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
}GBlockIs;

//////////// ESTRUCTURAS PARA TABLAS DE ARCHIVOS ////////////

typedef struct sac_global_fd_node{
	GFile* inodo;
	uint8_t numero_aperturas;
} GlobalFdNode;

typedef struct sac_process_fd_node{
	GlobalFdNode* globalFd;
} ProcessFdNode;

int main(int argc, char *argv[]);
size_t getFileSize(char* file);
void cargarDisco(char* diskName);

