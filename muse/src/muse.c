#include "muse.h"


int main(void) {

	leer_config();
	/*
	int cliente;
	int puerto_escucha;
	t_paquete paquete_recibido;

	puerto_escucha = escuchar(PUERTO);
	cliente = aceptarConexion(puerto_escucha);

	paquete_recibido = recibir_paquete(cliente);

	printf("string recibido: %s\n",obtener_string(paquete_recibido.parametros));

	close(cliente);
	*/

	printf("%d\n",PUERTO);
	printf("%d\n",TAM_MEMORIA);
	printf("%d\n",TAM_PAGINA);
	printf("%d\n",TAM_SWAP);

	return 0;
}
