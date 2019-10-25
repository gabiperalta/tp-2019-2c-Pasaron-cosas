#include <sac-server.h>


//////// FUNCIONES PRINCIPALES ////////
// FUNCIONES CON DIRECTORIOS

int crearDirectorio(const char *path, mode_t mode);
t_list* listarDirectorio(GFile *directorio);
int eliminarDirectorio(const char *path);


// FUNCIONES CON ARCHIVOS
int crearArchivo(const char *path, mode_t, dev_t);
int abrirArchivo(const char *path, struct fuse_file_info *);
int eliminarArchivo(const char *path);
int reposicionearPunteroDeArchivo(int filedes, off_t offset, int whence);


//////// FUNCIONES AUXILIARES ////////

// Modos de buscarInodoDirectorio

#define NORMAL 0
#define SIN_EL_ULTIMO 1
#define SIN_LOS_DOS_ULTIMOS 2

ptrGBloque punteroDirectorioPadre(char* path);
// Busca el inodo de un archivo, y retorna el puntero a su inodo. el inodo en si es devuelto en el parametro inodoDirectorio;
// En caso de que no exista el archivo, el inodo devuelto es igual a NULL y el retorno es 0
ptrGBloque buscarInodoArchivo(const char *path, int mode, GFile inodoArchivo);
GFile* buscarArchivoEnDirectorio(GFile *directorio, char* archivo);
bool elEstadoDelArchivoEs(GFile *archivo, uint8_t state);
int cantidadBloquesAsignados(ptrGBloque array[]);
bool entradaVacia(GDirEntry entrada);
uint8_t cantidadElementosCharAsteriscoAsterisco(char** array);
