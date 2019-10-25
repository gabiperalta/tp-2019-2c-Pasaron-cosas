#include "gestorDeMemoria.h"

int bloqueLibre(){
	int bloque = 0;
	bool ocupado;
	int tamanioBitarray;

	pthread_mutex_lock(&mutexBitmap);

	ocupado = bitarray_test_bit(bitmap, bloque);
	tamanioBitarray = bitarray_get_max_bit(bitmap);
	while((bloque < tamanioBitarray) && ocupado ){
		bloque++;
		ocupado = bitarray_test_bit(bitmap, bloque);
	}
	bitarray_set_bit(bitmap, bloque);

	pthread_mutex_unlock(&mutexBitmap);

	return bloque;
}

int inodoLibre(){
	int nInodo = 0;
	bool encontrado = false;
	struct sac_file_t *nodeTable = (struct sac_file_t*) myDisk;
	GFile node;

	node = nodeTable[nInodo];
	while(node->state == BORRADO && nInodo < NODE_TABLE_SIZE){
		nInodo++;
		node = nodeTable[nInodo];
	}
	if(node.state != BORRADO){
		return -1; // SIGNIFICA QUE NO HAY NINGUN INODO LIBRE
	}

	return nInodo;
}

void liberarBloque(int bloque){
	bitarray_clean_bit(bitmap, bloque);
}

int reservarInodo(int tipoDeArchivo){
	int inode;
	pthread_mutex_lock(mutexEscrituraInodeTable);

	inode = inodoLibre();

	// SETEAR EL STATUS DEL INODO CON tipoDeArchivo


	pthread_mutex_unlock(mutexEscrituraInodeTable);
}
