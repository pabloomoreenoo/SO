#include <stdio.h>
#include <stdlib.h>
// BIBLIOTECAS QUE TENEMOS QUE AGREGAR
#include <fcntl.h>    // open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <unistd.h>   // read, write, close

#include <sys/types.h> // mode_t, *struct stat*
#include <sys/stat.h> //mode_t, struct stat, lstat, S_ISREG, S_ISLINK* 
// ESTAS DOS ULTIMAS LIBRERIAS SON NECESARIAS PARA TRABAJAR CON ENLACES SIMBOLICOS


#define BUFFER_SIZE 512


/*
Cuando creamos un enlace simbolico se crea un nuevo inodo para el mismo, 
indicando que el archivo es del tipo enlace simbolico. El inodo contiene
la ruta(el nombre) del archivo o directorio al que apunta. Tambien se
crea una nueva entrada de directorio para el enlace simbolico

*/

void copy(int fdo, int fdd)
{

	ssize_t bytes_read; 
	char *buffer[BUFFER_SIZE]; 

	while((bytes_read = read(fdo, buffer, sizeof(char))) > 0){
		ssize_t bytes_written = write(fdd, buffer, bytes_read); 

		if(bytes_written == -1){
			perror("Error al escribir en el archivo copia"); 
			close(fdo);
			close(fdd); 
			exit(EXIT_FAILURE);
		}

		if(bytes_written != bytes_read){
			perror("Escritura incompleta en la copia del archivo");
			close(fdo);
			close(fdd); 
			exit(EXIT_FAILURE); 
		}
	}

	if(bytes_read == -1){
		perror("Error al leer del archivo original"); 
		close(fdo);
		close(fdd); 
		exit(EXIT_FAILURE);
	}

}

// esta funcion es basicamente la misma que la de arriba, es decir, la del ej1
void copy_regular(char *orig, char *dest)
{
	int fdo, fdd; 
	char buffer[BUFFER_SIZE]; 
	ssize_t bytes_read; 

	fdo = open(orig, O_RDONLY); 
	if(fdo == -1){
		printf("Error al abrir el archivo de lectura"); 
		exit(EXIT_FAILURE); 
		close(fdo); 
	}

	fdd = open(dest, O_WRONLY | O_TRUNC | O_CREAT); 
	if(fdd == -1){
		printf("Error al abrir el fichero de escritura"); 
		exit(EXIT_FAILURE);
		close(fdd); 
	}

	while((bytes_read = read(fdo, buffer, sizeof(char))) > 0){
		ssize_t bytes_written = write(fdd, buffer, bytes_read); 

		if(bytes_written == -1){
			perror("Error al escribir en el archivo copia"); 
			close(fdo);
			close(fdd); 
			exit(EXIT_FAILURE);
		}

		if(bytes_written != bytes_read){
			perror("Escritura incompleta en la copia del archivo");
			close(fdo);
			close(fdd); 
			exit(EXIT_FAILURE); 
		}
	}

	if(bytes_read == -1){
		perror("Error al leer del archivo original"); 
		close(fdo);
		close(fdd); 
		exit(EXIT_FAILURE);
	}

}

// esta funcion si que es totalmente distinta y podemos apreciar cambios 
// que tendremos que tener en cuenta para estudiar
void copy_link(char *orig, char *dest)
{
	struct stat statbuf; 
	char *link_target; 
	ssize_t link_size;

	// Reservamos memoria para la ruta a la que apunta en enlace
	link_size = statbuf.st_size + 1; // añadimos 1 para el caracter nulo
	link_target = malloc(link_size); 
	if(link_target == NULL){
		perror("Error al reservar memoria"); 
		exit(EXIT_FAILURE); 
	}

	// leer la ruta a la que apunta en enlace simbolico (readlink)
	ssize_t bytes_read = readlink(orig, link_target, link_size-1); // -1 porque readlink no añade el '\0'
	if(bytes_read == -1){
		perror("Error al usar readlink"); 
		free(link_target); // liberar memoria reservada antes de salir
		exit(EXIT_FAILURE);
	}
	link_target[bytes_read] = '\0'; // Añadimos el caracter nulo

	// CREAMOS EL NUEVO ENLACE SIMBOLICO
	if(symlink(link_target, dest) == -1){
		perror("Error al usar symlink"); 
		free(link_target); 
		exit(EXIT_FAILURE); 
	}

	free(link_target); //liberar la memoria despues de usar symlink
}

int main(int argc, char *argv[])
{
	// estructura stat para obtener info del archivo con lstat
	struct stat statbuf; 

	//int fdo, fdd; (Esto lo utilizamos dentro de las otras funciones)

	if (argc != 3){
		perror("Debes introducir 3 argumentos"); 
		exit(EXIT_FAILURE); 
	}

	/*fdo = open(argv[1], O_RDONLY); 
	if(fdo == -1){
		printf("Error al abrir el archivo de lectura"); 
		exit(EXIT_FAILURE); 
		close(fdo); 
	}

	fdd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT); 
	if(fdd == -1){
		printf("Error al abrir el fichero de escritura"); 
		exit(EXIT_FAILURE);
		close(fdd); 
	}*/

	// Obtenemos informacion sobre el archivo de origen
	// (lstat)
	if(lstat(argv[1], &statbuf) == -1){
		perror("Error al usar lstat"); 
		exit(EXIT_FAILURE); 
	}

	// comprobamos el tipo de archivo y actuar en consecuencia
	if(S_ISREG(statbuf.st_mode)){
		copy_regular(argv[1], argv[2]);
	} 
	else if(S_ISLNK(statbuf.st_mode)){
		copy_link(argv[1], argv[2]); 
	}
	else{
		fprintf(stderr, "Error: El archivo origen no es un archivo regular ni un enlace simbolico. \n"); 
		exit(EXIT_FAILURE); 
	}

	return 0;
}
