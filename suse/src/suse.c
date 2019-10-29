#include <suse.h>

int main(){
	puts("Proceso SUSE");
	archivo_config = config_create(PATH_CONFIG);
	char* linea;
	levantarSuse();

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
