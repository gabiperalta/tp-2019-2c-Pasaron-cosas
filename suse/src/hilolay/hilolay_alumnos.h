
//hay que lograr que a hilolay le hagan collbacks, tiene que inicializar hilolay.
//no hay main

//alguien le tiene que decir a hilolay nicializate, lo hace e programa que corre el hilolay init

//hillay no tiene el struct implenetado entonces implementa el init alumno . Entonces el rpograma llama al init de almuno que a su vez inicializa hilolay y usa el suse creat. Alumno le hace el callback a hilolay. El init esta en hilolay.h . Al hacer el callback hilolay conoce las funciones del struct de alumnos y puede e mpezar a implementarlas . El call back es cada funcion que es de inversion de control.

#include <suse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include "/home/utnso/tp-2019-2c-Pasaron-cosas/biblioteca/biblioteca_sockets.h"
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>

#ifndef hilolay_alumnos_h__
	#define hilolay_alumnos_h__

	/**
	 * TODO: Interface for alumnos (what they should implement in orde to make this work)
	 */
	typedef struct hilolay_operations {

		int (*suse_create) (int);
		int (*suse_schedule_next) (void);
		int (*suse_join) (int);
		int (*suse_close) (int);
		int (*suse_wait) (int, char *);
		int (*suse_signal) (int, char *);
	} hilolay_operations;

	hilolay_operations *main_ops;

	void init_internal(struct hilolay_operations*);

#endif // hilolay_alumnos_h__
