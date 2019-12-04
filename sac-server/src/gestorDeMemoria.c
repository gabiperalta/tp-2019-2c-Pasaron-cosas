#include "gestorDeMemoria.h"

ptrGBloque bloqueLibre(){
	ptrGBloque bloque = DATA_BLOCKS_START;
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

ptrGBloque inodoLibre(){
	int nInodo = 0;
	GFile *nodeTable = directorioRaiz();

	while((nodeTable+nInodo)->state != BORRADO && nInodo < NODE_TABLE_SIZE){
		nInodo ++;
	}
	if(nInodo >= NODE_TABLE_SIZE){
		return 0;
	}

	return nInodo + INODE_TABLE_START;
}

void liberarBloqueDeDatos(ptrGBloque bloque){
	bitarray_clean_bit(bitmap, bloque);
}

ptrGBloque reservarInodo(int tipoDeArchivo){
	ptrGBloque inode;
	GFile *nuevoInodo;

	pthread_mutex_lock(&mutexEscrituraInodeTable);

	inode = inodoLibre();

	if(inode){
		// SETEAR EL STATUS DEL INODO CON tipoDeArchivo
		nuevoInodo = (GFile*) obtenerBloque(inode);

		nuevoInodo->state = tipoDeArchivo;
	}

	pthread_mutex_unlock(&mutexEscrituraInodeTable);

	return inode;
}
