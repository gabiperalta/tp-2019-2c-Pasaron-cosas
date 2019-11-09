#include "sac-cli.h"


/** Get file attributes.
 *
 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
 * ignored.	 The 'st_ino' field is ignored except if the 'use_ino'
 * mount option is given.
 */
static int sac_cli_getattr( const char *path, struct stat *statRetorno ){
	int retorno = 0;

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// MANDO UNICAMENTE EL PATH, Y QUE EL SERVIDOR ME DEVUELVA LOS PARAMETROS QUE NECESITO
	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud.parametros, my_socket);


	// RECIBO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_ino = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_mode = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_size = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_blksize = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_blocks = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_mtim.tv_nsec = obtener_valor(paquete_respuesta.parametros );

	return retorno;
}


/** Read directory
 *
 * This supersedes the old getdir() interface.  New applications
 * should use this.
 *
 * The filesystem may choose between two modes of operation:
 *
 * 1) The readdir implementation ignores the offset parameter, and
 * passes zero to the filler function's offset.  The filler
 * function will not return '1' (unless an error happens), so the
 * whole directory is read in a single readdir operation.  This
 * works just like the old getdir() method.
 *
 */
static int sac_cli_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ){
	int retorno = 0;
	char* bufferAuxiliar;

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// MANDO UNICAMENTE EL PATH, Y QUE EL SERVIDOR ME DEVUELVA LOS PARAMETROS QUE NECESITO
	agregar_string(paquete_solicitud.parametros, path);
	agregar_string(paquete_solicitud.parametros, bufferAuxiliar);
	enviar_paquete(paquete_solicitud.parametros, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );
	bufferAuxiliar = obtener_string(paquete_respuesta.parametros);

	char** bufferAuxiliarSplitteado = string_split(bufferAuxiliar, ";");

	int tamanioBufferSpliteado = cantidadElementosCharAsteriscoAsterisco(bufferAuxiliarSplitteado);

	for(int i = 0; i < tamanioBufferSpliteado; i++){
		filler( buffer, bufferAuxiliarSplitteado[i], NULL, 0);
	}

	// liberarCharAsteriscoAsterisco(bufferAuxiliarSplitteado);
	// free(bufferAuxiliar); TODO LO DEJO COMENTADO PORQUE NO SE COMO VA A AFECTAR AL BUFFER FILLEADO

	return retorno;
}


/** Create a file node
 *
 * This is called for creation of all non-directory, non-symlink
 * nodes.  If the filesystem defines a create() method, then for
 * regular files that will be called instead.
 */
static int sac_cli_mknod(const char *path, mode_t mode, dev_t dev){
	int retorno = 0;


	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// MANDO UNICAMENTE EL PATH
	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud.parametros, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );



	return retorno;
}

/** File open operation
 *
 * No creation (O_CREAT, O_EXCL) and by default also no
 * truncation (O_TRUNC) flags will be passed to open(). If an
 * application specifies O_TRUNC, fuse first calls truncate()
 * and then open(). Only if 'atomic_o_trunc' has been
 * specified and kernel version is 2.6.24 or later, O_TRUNC is
 * passed on to open.
 *
 * Unless the 'default_permissions' mount option is given,
 * open should check if the operation is permitted for the
 * given flags. Optionally open may also return an arbitrary
 * filehandle in the fuse_file_info structure, which will be
 * passed to all file operations.
 *
 * Changed in version 2.2
 */
static int sac_cli_open(const char *path, struct fuse_file_info * file_info){
	int retorno = 0;

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// MANDO UNICAMENTE EL PATH
	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud.parametros, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );

	return retorno;
}

static int sac_cli_write(const char *path, const char *, size_t, off_t, struct fuse_file_info *){
	int retorno = 0;


	/** Write data to an open file
	 *
	 * Write should return exactly the number of bytes requested
	 * except on error.	 An exception to this is when the 'direct_io'
	 * mount option is specified (see read operation).
	 *
	 * Changed in version 2.2
	 */


	return retorno;
}


/** Read data from an open file
 *
 * Read should return exactly the number of bytes requested except
 * on EOF or error, otherwise the rest of the data will be
 * substituted with zeroes.	 An exception to this is when the
 * 'direct_io' mount option is specified, in which case the return
 * value of the read system call will reflect the return value of
 * this operation.
 *
 * Changed in version 2.2
 */
static int sac_cli_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi ){
	int retorno = 0;


	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	agregar_string( paquete_solicitud.parametros, path);
	agregar_string( paquete_solicitud.parametros, buffer);
	agregar_valor( paquete_solicitud.parametros, size);
	agregar_valor( paquete_solicitud.parametros, offset);
	enviar_paquete( paquete_solicitud.parametros, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor( paquete_respuesta.parametros );
	buffer = obtener_string( paquete_respuesta.parametros );


	return retorno;
}

/** Remove a file */
static int sac_cli_unlink(const char *path){
	int retorno = 0;

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud.parametros, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );


	return retorno;
}

/** Create a directory
 *
 * Note that the mode argument may not have the type specification
 * bits set, i.e. S_ISDIR(mode) can be false.  To obtain the
 * correct directory type bits use  mode|S_IFDIR
 * */
static int sac_cli_mkdir(const char *path, mode_t mode){
	int retorno = 0;

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud.parametros, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );


	return retorno;
}

/** Remove a directory */
static int sac_cli_rmdir(const char *path){
	int retorno = 0;

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud.parametros, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );


	return retorno;
}


static struct fuse_operations sac_cli_oper = {
		.getattr = sac_cli_getattr,
		.readdir = sac_cli_readdir,
		.open = sac_cli_open,
		.mknod = sac_cli_mknod,
		.write = sac_cli_write,
		.read = sac_cli_read,
		.unlink = sac_cli_unlink,
		.mkdir = sac_cli_mkdir,
		.rmdir = sac_cli_rmdir,
};






static struct fuse_opt fuse_options[] = {
		/*// Este es un parametro definido por nosotros
		CUSTOM_FUSE_OPT_KEY("--welcome-msg %s", welcome_msg, 0),

		// Estos son parametros por defecto que ya tiene FUSE
		FUSE_OPT_KEY("-V", KEY_VERSION),
		FUSE_OPT_KEY("--version", KEY_VERSION),
		FUSE_OPT_KEY("-h", KEY_HELP),
		FUSE_OPT_KEY("--help", KEY_HELP),
		FUSE_OPT_END,*/
};


uint8_t cantidadElementosCharAsteriscoAsterisco(char** array){
	uint8_t size;
	for(size = 0; array[size] != NULL; size++);
	return size;
}

void liberarCharAsteriscoAsterisco(char** array){
	string_iterate_lines(array, free);
	free(array);
	return;
}






int main(int argc, char *argv[]) {
	// Esta es la funcion principal de FUSE, es la que se encarga
	// de realizar el montaje, comuniscarse con el kernel, delegar todo
	// en varios threads
	 /* levantar un archivo de config
	 * conectarme a sac server
	 *
	 * guardas socket en variable global
	 */
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	t_config* archivo_config = config_create(PATH_CONFIG);
	char* ip_filesystem = config_get_string_value(archivo_config, "IP-FILESYSTEM");
	int puerto = config_get_int_value(archivo_config, "PUERTO");


	// Limpio la estructura que va a contener los parametros
	memset(&runtime_options, 0, sizeof(struct t_runtime_options));

	// Esta funcion de FUSE lee los parametros recibidos y los intepreta
	if (fuse_opt_parse(&args, &runtime_options, fuse_options, NULL) == -1){
		// error parsing options //
		perror("Invalid arguments!");
		return EXIT_FAILURE;
	}

	my_socket = conectarseA(ip_filesystem, puerto);

	if(my_socket == 0){
		printf("ERROR: Hubo un error al conectarse al servidor.");
	}

	return fuse_main(args.argc, args.argv, &hello_oper, NULL);
}
