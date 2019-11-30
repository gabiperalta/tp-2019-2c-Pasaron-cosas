#include "sac-cli.h"


char* formatearPath(char* path){
	char* path_formateado = string_substring_from(path, path_length);
	printf("El path formateado es: %s\n", path_formateado);
	return path_formateado;
}

/** Get file attributes.
 *
 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
 * ignored.	 The 'st_ino' field is ignored except if the 'use_ino'
 * mount option is given.
 */
static int sac_cli_getattr( const char *path, struct stat *statRetorno ){
	int retorno = 0;

	//char* path_formateado = formatearPath(path);

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// MANDO UNICAMENTE EL PATH, Y QUE EL SERVIDOR ME DEVUELVA LOS PARAMETROS QUE NECESITO
	// agregar_string(paquete_solicitud.parametros, path_formateado);
	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud, my_socket);


	// RECIBO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_ino = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_mode = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_size = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_blksize = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_blocks = obtener_valor(paquete_respuesta.parametros );
	statRetorno->st_mtim.tv_nsec = obtener_valor(paquete_respuesta.parametros );

	printf("statRetorno: \n"
			"ino: %i \n"
			"mode: %i \n"
			"size: %i \n"
			"blksize: %i \n"
			"blocks: %i \n"
			"mtim: %i \n", statRetorno->st_ino, statRetorno->st_mode, statRetorno->st_size,
			statRetorno->st_blksize, statRetorno->st_blocks, statRetorno->st_mtim.tv_nsec);

	//free(path_formateado);

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
	(void) offset;
	(void) fi;

	//char* path_formateado = formatearPath(path);

	if (strcmp(path, "/") != 0)
			return -ENOENT;

	int retorno = 0;
	char* bufferAuxiliar;

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// MANDO UNICAMENTE EL PATH, Y QUE EL SERVIDOR ME DEVUELVA LOS PARAMETROS QUE NECESITO
	//agregar_string(paquete_solicitud.parametros, path_formateado);
	agregar_string(paquete_solicitud.parametros, path);
	agregar_string(paquete_solicitud.parametros, bufferAuxiliar);
	enviar_paquete(paquete_solicitud, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );
	bufferAuxiliar = obtener_string(paquete_respuesta.parametros);

	char** bufferAuxiliarSplitteado = string_split(bufferAuxiliar, ";");

	int tamanioBufferSpliteado = cantidadElementosCharAsteriscoAsterisco(bufferAuxiliarSplitteado);

	for(int i = 0; i < tamanioBufferSpliteado; i++){
		filler( buffer, bufferAuxiliarSplitteado[i], NULL, 0);
	}

	//free(path_formateado);

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

	//char* path_formateado = formatearPath(path);

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// MANDO UNICAMENTE EL PATH
	// agregar_string(paquete_solicitud.parametros, path_formateado);
	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );

	//free(path_formateado);

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

	// char* path_formateado = formatearPath(path);

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// MANDO UNICAMENTE EL PATH
	// agregar_string(paquete_solicitud.parametros, path_formateado);
	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );

	// free(path_formateado);

	return retorno;
}


/** Write data to an open file
 *
 * Write should return exactly the number of bytes requested
 * except on error.	 An exception to this is when the 'direct_io'
 * mount option is specified (see read operation).
 *
 * Changed in version 2.2
 */
static int sac_cli_write( const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi ){
	int retorno = 0;

	// char* path_formateado = formatearPath(path);

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// agregar_string( paquete_solicitud.parametros, path_formateado);
	agregar_string( paquete_solicitud.parametros, path);
	agregar_string( paquete_solicitud.parametros, buffer);
	agregar_valor( paquete_solicitud.parametros, size);
	agregar_valor( paquete_solicitud.parametros, offset);
	enviar_paquete( paquete_solicitud, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor( paquete_respuesta.parametros );

	// free(path_formateado);

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

	// char* path_formateado = formatearPath(path);

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// agregar_string( paquete_solicitud.parametros, path_formateado);
	agregar_string( paquete_solicitud.parametros, path);
	agregar_string( paquete_solicitud.parametros, buffer);
	agregar_valor( paquete_solicitud.parametros, size);
	agregar_valor( paquete_solicitud.parametros, offset);
	enviar_paquete( paquete_solicitud, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor( paquete_respuesta.parametros );
	buffer = obtener_string( paquete_respuesta.parametros );

	// free(path_formateado);

	return retorno;
}

/** Remove a file */
static int sac_cli_unlink(const char *path){
	int retorno = 0;

	// char* path_formateado = formatearPath(path);

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// agregar_string(paquete_solicitud.parametros, path_formateado);
	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );

	// free(path_formateado);

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

	// char* path_formateado = formatearPath(path);

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// agregar_string(paquete_solicitud.parametros, path_formateado);
	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );

	// free(path_formateado);

	return retorno;
}

/** Remove a directory */
static int sac_cli_rmdir(const char *path){
	int retorno = 0;

	// char* path_formateado = formatearPath(path);

	t_paquete paquete_solicitud = {
			.header = FUSE_GETATTR,
			.parametros = list_create()
	};

	// agregar_string(paquete_solicitud.parametros, path_formateado);
	agregar_string(paquete_solicitud.parametros, path);
	enviar_paquete(paquete_solicitud, my_socket);

	// RECIVO LA RESPUESTA DEL SAC-SERVER
	t_paquete paquete_respuesta = recibir_paquete(my_socket);

	retorno = obtener_valor(paquete_respuesta.parametros );

	// free(path_formateado);

	return retorno;
}


static struct fuse_operations sac_cli_oper = {
		.getattr = sac_cli_getattr,
		.open = sac_cli_open,
		.mknod = sac_cli_mknod,
		// .readdir = sac_cli_readdir,
		.read = sac_cli_readdir,
		.write = sac_cli_write,
		.read = sac_cli_read,
		.unlink = sac_cli_unlink,
		.mkdir = sac_cli_mkdir,
		.rmdir = sac_cli_rmdir,
};


uint8_t cantidadElementosCharAsteriscoAsterisco(char** array){
	uint8_t size;
	for(size = 0; array[size] != NULL; size++);
	return size;
}

void liberarCharAsteriscoAsterisco(char** array){
	string_iterate_lines(array, (void*)free);
	free(array);
	return;
}

#define DEFAULT_FILE_CONTENT "Hello World!\n"

/*
 * Este es el nombre del archivo que se va a encontrar dentro de nuestro FS
 */
#define DEFAULT_FILE_NAME "hello"

/*
 * Este es el path de nuestro, relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME



/*
 * Esta es una estructura auxiliar utilizada para almacenar parametros
 * que nosotros le pasemos por linea de comando a la funcion principal
 * de FUSE
 */
struct t_runtime_options {
	char* welcome_msg;
} runtime_options;

/*
 * Esta Macro sirve para definir nuestros propios parametros que queremos que
 * FUSE interprete. Esta va a ser utilizada mas abajo para completar el campos
 * welcome_msg de la variable runtime_options
 */
#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }

enum {
	KEY_VERSION,
	KEY_HELP,
};

static struct fuse_opt fuse_options[] = {
		CUSTOM_FUSE_OPT_KEY("--welcome-msg %s", welcome_msg, 0),
		FUSE_OPT_KEY("-V", KEY_VERSION),
		FUSE_OPT_KEY("--version", KEY_VERSION),
		FUSE_OPT_KEY("-h", KEY_HELP),
		FUSE_OPT_KEY("--help", KEY_HELP),
		FUSE_OPT_END,
};

int main(int argc, char *argv[]) {
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	printf("hola\n");

	t_config* archivo_config = config_create(PATH_CONFIG);

	printf("hola\n");
	char* ip_filesystem = config_get_string_value( archivo_config, "IP-FILESYSTEM");

	printf("hola\n");
	int puerto = config_get_int_value(archivo_config, "PUERTO");

	printf("hola\n");

	// ME CONECTO A SAC-SERVER
	my_socket = conectarseA(ip_filesystem, puerto); // guardas socket en variable global

	printf("hola\n");

	if(my_socket == 0){
		printf("ERROR: Hubo un error al conectarse al servidor.");
		return -1;
	}

	path_length = string_length(argv[1]);

	// Limpio la estructura que va a contener los parametros
		memset(&runtime_options, 0, sizeof(struct t_runtime_options));

		// Esta funcion de FUSE lee los parametros recibidos y los intepreta
		if (fuse_opt_parse(&args, &runtime_options, fuse_options, NULL) == -1){
			/** error parsing options */
			perror("Invalid arguments!");
			return EXIT_FAILURE;
		}

		// Si se paso el parametro --welcome-msg
		// el campo welcome_msg deberia tener el
		// valor pasado
		if( runtime_options.welcome_msg != NULL ){
			printf("%s\n", runtime_options.welcome_msg);
		}



	// Esta es la funcion principal de FUSE
	return fuse_main(args.argc, args.argv, &sac_cli_oper, NULL);
}
