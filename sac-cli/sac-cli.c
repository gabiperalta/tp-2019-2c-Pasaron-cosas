#include "sac-cli.h"

static int sac_cli_getattr( const char *path, struct stat *st ){
	int retorno = 0;


	return retorno;
}

static int sac_cli_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ){
	int retorno = 0;


	return retorno;
}

static int sac_cli_mknod(){
	int retorno = 0;


	return retorno;
}

static int sac_cli_write(){

}

static int sac_cli_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi ){
	int retorno = 0;

	return retorno;
}

sac_cli_unlink

sac_cli_mkdir

sac_cli_rmdir

sac_cli_dirfd

static struct fuse_operations sac_cli_oper = {
		.getattr = sac_cli_getattr,
		.readdir = sac_cli_readdir,
		//.open = hello_open,
		.mknod = sac_cli_mknod,
		.write = sac_cli_write,
		.read = sac_cli_read,
		.unlink = sac_cli_unlink,
		.mkdir = sac_cli_mkdir,
		.rmdir = sac_cli_rmdir,
		.dirfd = sac_cli_dirfd
};
