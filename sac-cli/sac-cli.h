#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>


/* 	La utilidad del sac server es la de servir como comunicacion entre el proceso que quiere hacer operaciones
 * 	con archivos y directorios dentro del punto de montaje, y el Sac-servidor, que actuara como el
 * 	Filesystem centralizado del tp. De esta manera que las solicitudes al sistema realizados por el proceso seran
 * 	delegados al Sac-servidor, y este sera el encargado de resolver esta solicitud.
 */

// GENERALES
static int sac_cli_getattr( const char *path, struct stat *st );
static int sac_cli_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi );

// ARCHIVOS
static int sac_cli_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi );


// DIRECTORIOS

