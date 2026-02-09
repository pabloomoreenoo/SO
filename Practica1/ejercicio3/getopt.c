#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include "defs.h"


char * progname;

/**
 * @brief  Print usage of the tool
 **/
void usage() {
	printf("Usage: %s [ options ] title \n",progname);
	printf("\n\toptions:\n");
	printf("\t\t-h: display this help message\n");
	printf("\t\t-e: print even numbers instead of odd  (default)\n");
	printf("\t\t-l lenght: lenght of the sequence to be printed\n");
	printf("\t\ttitle: name of the sequence to be printed\n");
}

/**
 * @brief  Prints a sequence of odd or even numbers in stdout
 *
 * @param lenght
 * @param type
 * @param title
 **/
void display_numbers ( int lenght, parity_t type, char * title) {
	
	int i,n;
	int first = (type == ODD )? 1 : 2;
	
	printf("Title: %s\n",title);
	for (i=0,n=first; i<lenght; i++,n+=2) {
		printf("%d ",n);
	}
	printf("\n");
}



int main(int argc, char *argv[])
{
	int  opt;
	struct options options;

	progname = argv[0];

	/* Initialize default values for options */
	options.par_mode = ODD;
	options.lenght= 10;
	options.title = NULL;

	/* Parse command-line options */
	while((opt = getopt(argc, argv, "hel:")) != -1) { // entre comillas apuntamos los parametros que puede recibir,(ponemos l: porque
		switch(opt) {                              // el parametro l va acompañado de otro que es el valor de la longitud)
		case 'h':
			usage();
			exit(0);
		case 'e':
			options.par_mode = EVEN; 
			break; 
		case 'l':
							// strtol() se utiliza para convertir una cadena a un entero largo
			options.lenght = strtol(optarg, NULL, 10); // optarg representa el argumento de -l, NULL(puntero que no necesitamos), 
			                                              // y 10 es la base numerica
			if(options.lenght <= 0){
				fprintf(stderr, "Error: Length must be a positive number \n"); 
				exit(EXIT_FAILURE); 
			}
			break; 													  

		default:
			exit(EXIT_FAILURE);
		}
	}

	// optind CONTIENE EL INDICE DEL PRIMER ARGUMENTO QUE NO FUE PROCESADO POR getopt()

	/* There should be one extra argument (the title of the sequence) */
	if (optind >= argc) { // nos ayuda a comprobar que hemos introducido los argumentos suficientes
		fprintf(stderr, "Invalid title\n");
		usage();
		exit(EXIT_FAILURE);
	}
	
	/* Fill options.title with the corresponding element of argv */
	options.title = argv[optind]; // el argumento que vaya en 2 posicion va a ser el titulo

    /* Call display_numbers */
	display_numbers (options.lenght, options.par_mode, options.title);
	return 0;
}
