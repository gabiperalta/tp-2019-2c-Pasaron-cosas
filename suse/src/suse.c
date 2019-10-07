#include <suse.h>

int main(){
	puts("Proceso SUSE");
	char* linea;
	t_config * archivo_config = config_create(PATH_CONFIG);
	inicializarColaNew();
	inicializarColaReady();
	pasarDeNewAReady();

	while(1){
		linea = readline(">");
		if(linea)
			add_history(linea);
		if(!strncmp(linea,"exit",4)){
			free(linea);
			break;
		}
	}
	config_destroy(archivo_config);
	return 0;
}
