#include "sac-server.h"


//////// FUNCIONES PRINCIPALES ////////

// FUNCIONES GENERALES

int myGetattr( const char *path, struct stat *statRetorno );

// FUNCIONES CON DIRECTORIOS

int crearDirectorio(const char *path );

int eliminarDirectorio(const char *path);

int myReaddir( const char *path, void *buffer );

// FUNCIONES CON ARCHIVOS
int crearArchivo(const char *path );

uint8_t abrirArchivo(const char *path, int socketProceso);

int leerArchivo( const char *path, char *buffer, size_t size, off_t offset );

int eliminarArchivo(const char *path);

int cerrarArchivo(const char *path); // TODAVIA NO SE QUE PARAMETROS LLEVA


//////// FUNCIONES AUXILIARES ////////

// Modos de buscarInodoDirectorio

#define NORMAL 0
#define SIN_EL_ULTIMO 1
#define SIN_LOS_DOS_ULTIMOS 2

t_list* listarDirectorio(GFile *directorio);

// Busca el inodo de un archivo, y retorna el puntero a su inodo. el inodo en si es devuelto en el parametro inodoDirectorio;
// En caso de que no exista el archivo, el inodo devuelto es igual a NULL y el retorno es 0
ptrGBloque buscarInodoArchivo(const char *path, int mode, GFile *inodoArchivo);

ptrGBloque buscarArchivoEnDirectorio(GFile *directorio, char* archivo);

bool elEstadoDelArchivoEs(GFile *archivo, uint8_t state);

GFile *directorioRaiz();

int cantidadBloquesAsignados(ptrGBloque array[]);

bool entradaVacia(GDirEntry entrada);

GDirEntry *buscarEntrada(ptrGBloque directorioPadre, ptrGBloque Archivo);

int reservarEntrada(ptrGBloque directorioPadre, ptrGBloque punteroAInodo, char* nombreArchivo);

void borrarEntrada(ptrGBloque directorioPadre, ptrGBloque Archivo);

int escribirEnBloque(ptrGBloque bloqueDestino, GBlock *contenido);

GBlock *obtenerBloque(ptrGBloque bloque);

bool noTieneHijos(ptrGBloque punteroAInodo);

void liberarBloquesAsignados(ptrGBloque blocks[]);

bool estaAbierto(ptrGBloque punteroAlInodo);

uint8_t agregarAListaDeArchivosDelProceso(GlobalFdNode* fdNode, int socketProceso);

void sacarArchivoDeTablaGlobal(GlobalFdNode* fdNode);

int sacarDeLaListaDeArchivosDelProceso(GlobalFdNode* fdNode, int socketProceso);

uint8_t obtenerFD(t_list* listaDeArchivosAbiertos);

GDirectoryBlock *asignarBloqueDeDirectorio(GFile* directorio);

void inicializarPrimerasEntradas(GDirectoryBlock* bloqueDeDirectorio, ptrGBloque punteroSelf, ptrGBloque punteroPadre);


//////////////////////////////////////////////////
// FUNCIONES AUXILIARES PARA EL MANEJO DE ARCHIVOS
void posicionEnArchivo(uint32_t offset, FileOffset* offsetDelArchivo);

void leerBloques(GFile* inodoArchivo, char* buffer, FileOffset* offsetInicial, FileOffset* offsetFinal);

//////////////// MANEJO char** ///////////////////
uint8_t cantidadElementosCharAsteriscoAsterisco(char** array);

void liberarCharAsteriscoAsterisco(char** array);

int maximo(int unNumero, int otroNumero);
uint32_t minimo(uint32_t unNumero, uint32_t otroNumero);
