#include <stdio.h> // para fopen, fclose, fprintf, fwrite, perror
#include <stdlib.h> // para EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // para strlen
#include <err.h>

int main(int argc, char* argv[])
{
	
	FILE *file; 
	int i; 
	
	if(argc < 2){ // comprobamos si le estamos pasando 2 args
		fprintf(stderr, "Uso: %s [...] \n", argv[0]); 
		exit(EXIT_FAILURE); 
	}

	if((file = fopen(argv[1], "wb")) == NULL){ // comprobamos que el archivo que le hemos pasado se pueda abrir correctamente
		perror("Error al escribir en el archivo");
		exit(EXIT_FAILURE);
	}


	// ITERAMOS SOBRE LOS ARGUMENTOS RESTANTES (las cadenas de caracteres)

	for(i=2; i<argc; i++){
		fwrite(argv[i], sizeof(char), strlen(argv[i]), file);
		fwrite("\0", sizeof(char), 1, file); // escribimos el caracter nulo
	}

	fclose(file); 


	return EXIT_SUCCESS;
}
