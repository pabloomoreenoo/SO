#include <stdio.h> // printf, fprintf, perror
#include <stdlib.h> // exit, EXIT_FAILURE, EXIT_SUCCESS, malloc
//LIBRERIAS QUE TENEMOS QUE AÑADIR
#include <sys/types.h> // struct stat, lstat
#include <sys/stat.h> // struct stat, lstat, S_ISDIR
#include <unistd.h> // lstat
#include <dirent.h>  // opendir, readdir, closedir, DIR, struct dirent
#include <string.h>  // strcmp
#include <errno.h>   // errno
 


/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);
int get_size(char *fname, size_t *blocks);


/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks) // FUNCION RECURSIVA PARA OBTENER EL TAMAÑO
{
	struct stat statbuf; 

	//1. Obtener informacion sobre el archivo/directorio (lstat)
	if(lstat(fname, &statbuf) == -1){
		perror("Error al usar lstat"); 
		return -1; 
	}

	//2. Si es un archivo regular, añadir su tamaño a *blocks
	if(S_ISREG(statbuf.st_mode)){
		*blocks += statbuf.st_blocks; 
		return 0;
	}

	//3. Si es un directorio, llamar a get_size_dir
	if(S_ISDIR(statbuf.st_mode)){
		get_size_dir(fname, blocks); 
	}

	return 0; 
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks)
{
	DIR *dir; 
	struct dirent *entry; 
	char path[1024]; // Buffer para construir rutas completas
	int ret_code = 0; 

	//1. Con esto abrimos el directorio y vemos si hay algun error
	dir = opendir(dname); 
	if(dir == NULL){
		perror("Error al abrir el directorio"); 
		return -1; 
	}

	//2. Leer las entradas del directorio
	errno = 0; // Inicializamos errno antes de readdir
	while((entry = readdir(dir)) != NULL){

		//3. Ignorar . y .. para no caer en bucles infinitos que esten accediendo siempre de padre a hijo y viceversa
		if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){

				//4. Construimos la ruta completa del archivo/subdirectorio que se guarda en path (que hace la funcion del buffer de los ejs anteriores)
				snprintf(path, sizeof(path), "%s/%s", dname, entry->d_name);

				//5. Llamamos recursivamente a get_size
				if(get_size(path, blocks) == -1){
					ret_code = -1; 
				}

		}
	}

	//comprobamos errores
	if(errno != 0){
		perror("Error al leer el directorio"); 
		ret_code = -1; 
	}

	// cerramos el directorio
	if(closedir(dir) == -1){
		perror("Error al cerrar el directorio");
		ret_code = -1; 
	}
	
	return ret_code; 
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{
	size_t total_blocks = 0; 
	int ret_code = 0; 
	
	//1. Comprobar que se han intorducido bien los argumentos
	if(argc < 2){
		fprintf(stderr, "Uso: %s <archivo1> [<archivo2> ...]\n", argv[0]); 
		exit(EXIT_FAILURE); 
	}

	//2. Iterar sobre los argumentos introducidos
	for(int i=1; i<argc; i++){
		total_blocks = 0; // Reseteamos el contador de bloques para cada archivo/directorio

		//3. Llamar a get_size para obtener el tamaño en bloques
		if(get_size(argv[i], &total_blocks) == -1){
			ret_code = -1; // Si ha habido fallo, lo guarda
			continue;
		}

		//4. Convertir a kilobytes e imprimir
		printf("%zuK %s\n", total_blocks/2, argv[i]);   // %zu: especificador de formato para un entero sin signo de tamaño variable(size_t)
	}													// que es el tipo de kilobytes

	return ret_code;
}
