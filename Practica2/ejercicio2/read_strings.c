#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)  // lee una cadena de carcateres que termina en \0 del fichero cuyo descriptor se pasa como parametro y reserva
{                        // dinamicamente la cantidad de memoria adecuada para la cadena leida y devolviendo dicha cadena

	char ch; // Variable para almacenar cada caracter leido del archivo
	int length = 0; // longitud de la cadena leida
	long start_pos = ftell(file);  // Almacenar la posicion inicial del archivo

	//calcular la longitud de la cadena leyendo caracter a caracter hasta encontrar '\0'(el caracter nulo)
	while(fread(&ch, sizeof(char), 1, file) == 1 && ch != '\0'){
		length++; 
	}

	if(ch != '\0'){ // si no encontramos un caracter nulo significa que ha habido un error 
		return NULL;
	}

	// restaurar el puntero de posicion al inicio de la cadena
	fseek(file, start_pos, SEEK_SET); 

	// asignar memoria para almacenar la cadena, incluyendo el carcater nulo
	char* str = (char*)malloc((length +1) * sizeof(char));

	if(str == NULL){ // si malloc falla, muestra un error
		return NULL; 
	}

	// leemos la cadena completa del archivo
	if(fread(str, sizeof(char), length+1, file) != (length + 1)){
		free(str); // libera la memoria si ocurre un error al leer
		return NULL;
	}


	return str; // devuelve el puntero a la cadena leida	
}

int main(int argc, char *argv[])
{
	FILE *file; 
	char *str; 
	
	if(argc < 2){
		fprintf(stderr, "Error, se requiren l menos 2 parametros \n", argv[0] );
		exit(EXIT_FAILURE); 
	}

	file = fopen(argv[1], "r"); 
	if(file == NULL){
		perror("Error al abrir el archivo de lectura"); 
		exit(EXIT_FAILURE); 
	}

	while((str = loadstr(file)) != NULL){ // mientras lea cadenas las imprime
		printf("%s\n", str); 
		free(str); // una vez hemos impreso las cadenas liberamos la memoria
	}

	if(ferror(file)){ // comprobamos si hay algun error
		perror("Error al leer el archivo"); 
		fclose(file); 
		exit(EXIT_FAILURE); 
	}
	
	fclose(file); 

	return EXIT_SUCCESS; 
}
