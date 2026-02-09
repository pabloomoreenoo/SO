#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	//int c,ret;
	char buffer[BUFFER_SIZE]; // buffer para almacenar la cadena de caracteres
	size_t bytes_read;         // contador para ir guardando los bytes leidos
	

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */   // marcamos que el open file es el segundo argumento que pasamos
	if ((file = fopen(argv[1], "rb")) == NULL) // agregamos la b ("rb") que significa binario para evitar problemas
		err(2,"The input file %s could not be opened",argv[1]);

	/* Read file byte by byte */
	while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
		
		/* Print byte to stdout */   //CODIGO ANTES DE LA MODIFICACION
		/*ret=putc((unsigned char) c, stdout);

		if (ret==EOF){
			fclose(file);
			err(3,"putc() failed!!");
		}*/

		if(fwrite(buffer, sizeof(char), bytes_read, stdout) != bytes_read){ // stdout es el outfile que en este caso es la consola
			perror("Error al escribir en la salida estandar"); 
			fclose(file); 
			exit(EXIT_FAILURE); 
		}
	}

	if(ferror(file)){
		perror("Error al leer el archivo"); 
		fclose(file); 
		exit(EXIT_FAILURE);
	}

	fclose(file);
	return 0;
}
