#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include "biblioteca_sockets.h"


/* 	La utilidad del sac server es la de servir como comunicacion entre el proceso que quiere hacer operaciones
 * 	con archivos y directorios dentro del punto de montaje, y el Sac-servidor, que actuara como el
 * 	Filesystem centralizado del tp. De esta manera que las solicitudes al sistema realizados por el proceso seran
 * 	delegados al Sac-servidor, y este sera el encargado de resolver esta solicitud.
 */

#define PATH_CONFIG "/home/utnso/tp-2019-2c-Pasaron-cosas/sac-cli/Config"

int my_socket;

// GENERALES
static int sac_cli_getattr( const char *path, struct stat *st );
static int sac_cli_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi );

// ARCHIVOS
static int sac_cli_mknod(const char *, mode_t, dev_t);
static int sac_cli_open(const char *, struct fuse_file_info *);
static int sac_cli_write(const char *, const char *, size_t, off_t, struct fuse_file_info *);
static int sac_cli_read( const char *, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi ); // aqui no tendria que tener como parametro un FD?
static int sac_cli_unlink(const char *path);

// DIRECTORIOS

static int sac_cli_mkdir(const char *path, mode_t mode);
static int sac_cli_rmdir(const char *path);

// FUNCIONES AUXILIARES

uint8_t cantidadElementosCharAsteriscoAsterisco(char** array);
void liberarCharAsteriscoAsterisco(char** array);
