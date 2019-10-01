#include "muse.h"


int main(void) {

	int cliente;
	int puerto_escucha;
	t_paquete paquete_recibido;

	puerto_escucha = escuchar(PUERTO);
	cliente = aceptarConexion(puerto_escucha);

	paquete_recibido = recibir_paquete(cliente);

	printf("valor  recibido: %d\n",obtener_valor(paquete_recibido.parametros));
	printf("string recibido: %s\n",obtener_string(paquete_recibido.parametros));

	close(cliente);

	return 0;
}
