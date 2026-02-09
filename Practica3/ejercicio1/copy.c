#include <stdio.h>
#include <stdlib.h>
// BIBLIOTECAS QUE TENEMOS QUE AGREGAR
#include <fcntl.h>    // open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <unistd.h>   // read, write, close

#include <sys/types.h> // mode_t
#include <sys/stat.h> //mode_t


#define BUFFER_SIZE 512

void copy(int fdo, int fdd)
{
	char *buffer[BUFFER_SIZE]; 
	ssize_t bytes_read;
	
	while((bytes_read = read(fdo, buffer, sizeof(buffer))) > 0){
		ssize_t bytes_written = write(fdd, buffer, bytes_read); 

		if(bytes_written == -1){
			perror("Error al escribir en el fichero destino"); 
			exit(EXIT_FAILURE); 
		}

		if(bytes_written != bytes_read){
			perror("Error, escritura incompleta"); 
			close(fdo); 
			close(fdd);
			exit(EXIT_FAILURE); 
		}
	}

	if(bytes_read == -1){
		perror("Error al leer del archivo origen");
		exit(EXIT_FAILURE);
	}

}

int main(int argc, char *argv[])
{
	int fdo; 
	int fdd; 

	if(argc != 3){ // compruebo que le estoy pasando 3 argumentos
		perror("Error, debes introducir 3 argumentos"); 
		exit(EXIT_FAILURE); 
	}

	fdo = open(argv[1], O_RDONLY);  // compruebo si se puede abrir bien el fichero origen
	if(fdo == -1){
		perror("Error al abrir el fichero origen"); 
		close(fdo); 
		exit(EXIT_FAILURE); 
	}

	fdd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644); // compruebo si se puede abrir bien el fcihero destino
	if(fdd == -1){                                   // y le doy permisos rw-r--r--
		perror("Error al abrir el fichero destino"); 
		close(fdd); 
		exit(EXIT_FAILURE); 
	}
	

	copy(fdo, fdd); 

	close(fdo); // cierro los ficheros una vez terminado
	close(fdd); 

	return EXIT_SUCCESS;
}
