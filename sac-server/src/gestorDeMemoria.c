#include "gestorDeMemoria.h"

int bloqueLibre(){
	t_bitarray bitmap;	// DEBO VER COMO CONSIGO EL BITMAP DEL ARCHIVO... POSIBLEMENTE HAGA UN DOBLE MMAP DEL ARCHIVO,
					    //	UNO EN GBLOQUES Y OTRO PARA EL BITARRAY SOLO
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
