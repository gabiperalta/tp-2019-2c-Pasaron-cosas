#include <sac-server.c>


//////// FUNCIONES PRINCIPALES ////////
// FUNCIONES CON DIRECTORIOS

int crearDirectorio(const char *path, mode_t mode);
t_list* listarDirectorio(GFile *directorio);





//////// FUNCIONES AUXILIARES ////////

// Modos de buscarInodoDirectorio

#define NORMAL 0
#define SIN_EL_ULTIMO 1

void buscarInodoDirectorio(const char *path, int mode);
GFile* buscarArchivoEnDirectorio(GFile *directorio, char* archivo);
bool elEstadoDelArchivoEs(GFile *archivo, uint8_t state);
int cantidadBloquesAsignados(ptrGBloque array[]);
bool entradaVacia(GDirEntry entrada);
uint8_t cantidadElementosCharAsteriscoAsterisco(char** array);
