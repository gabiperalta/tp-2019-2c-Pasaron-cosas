#include <stdint.h>
#include <inttypes.h>
#include <commons/bitarray.h>

#define BLOCK_SIZE 4096
#define MAGIC_NUMBER_NAME 3
#define MAX_FILENAME_LENGTH 71
#define MAX_FILE_NUMBER 1024
#define BITMAP_START_BLOCK 1
#define BITMAP_SIZE_IN_BLOCKS 1
#define NODE_TABLE_SIZE 1024  // EN BLOQUES
#define NEXT_BLOCK(A) A + 1

typedef uint32_t ptrGBloque;

typedef struct sac_block {
	unsigned char bytes[BLOCK_SIZE];
} GBlock;

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

