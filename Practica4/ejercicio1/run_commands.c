#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//LIBRERIAS QUE HAY QUE METER
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> // para hacer la funcion pwait() en los procesos

pid_t launch_command(char** argv){ // recibe una cadena de caracteres que representa el comando y sus argumentos. 
    pid_t pid = fork();             // crea el proceso hijo y ejecuta el comando en el

    if(pid == -1){
        perror("fork"); 
        exit(EXIT_FAILURE); 
    }

    if(pid == 0){
        //Proceso hijo
        execvp(argv[0], argv); // se ejecuta el comando especificado, por lo que se deja de ejecutar lo que sea que estuviera antes 
        
        // si la funcion execvp() retorna, hay un error, porque si tiene exito el proceso hijo deja de ejecutar este codigo y empeiza 
        perror("execvp");                                                             // a ejecutar el nuevo programa
        exit(EXIT_FAILURE); 
    }

    return pid; // se devuelve el pid del hijo
}



char **parse_command(const char *cmd, int* argc) {
    // Allocate space for the argv array (initially with space for 10 args)
    size_t argv_size = 10;
    const char *end;
    size_t arg_len; 
    int arg_count = 0;
    const char *start = cmd;
    char **argv = malloc(argv_size * sizeof(char *));

    if (argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (*start && isspace(*start)) start++; // Skip leading spaces

    while (*start) {
        // Reallocate more space if needed
        if (arg_count >= argv_size - 1) {  // Reserve space for the NULL at the end
            argv_size *= 2;
            argv = realloc(argv, argv_size * sizeof(char *));
            if (argv == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }

        // Find the start of the next argument
        end = start;
        while (*end && !isspace(*end)) end++;

        // Allocate space and copy the argument
        arg_len = end - start;
        argv[arg_count] = malloc(arg_len + 1);

        if (argv[arg_count] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        strncpy(argv[arg_count], start, arg_len);
        argv[arg_count][arg_len] = '\0';  // Null-terminate the argument
        arg_count++;

        // Move to the next argument, skipping spaces
        start = end;
        while (*start && isspace(*start)) start++;
    }

    argv[arg_count] = NULL; // Null-terminate the array

    (*argc)=arg_count; // Return argc

    return argv;
}

/*int main(int argc, char *argv[]) {   // MAIN INICIAL
    char **cmd_argv;
    int cmd_argc;
    int i;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"command\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    cmd_argv=parse_command(argv[1],&cmd_argc);

    // Print parsed arguments
    printf("argc: %d\n", cmd_argc);
    for (i = 0; cmd_argv[i] != NULL; i++) {
        printf("argv[%d]: %s\n", i, cmd_argv[i]);
        free(cmd_argv[i]);  // Free individual argument
    }

    free(cmd_argv);  // Free the cmd_argv array

    return EXIT_SUCCESS;
}*/

int main(int argc, char *argv[]){
    int opt; 
    int execute_flag = 0;  // Variable que utilizaremos como bandera, si se usa -x se pone a 1 y si no a 0
    char *command_file = NULL; // para cuando ejecutemos con la opcion -s, esta variable apuntara al nombre del archivo

    while((opt = getopt(argc, argv, "xs:")) != -1){
        switch(opt){
            case 'x':
                execute_flag = 1; 
                break; 
            case 's':
                command_file = optarg; 
                break; 
            case '?':
                fprintf(stderr, "Usage: %s [-x[comando]] [-s <archivo_comandos]\n", argv[0]);
                exit(EXIT_FAILURE);
            default:
                fprintf(stderr, "Usage: %s [-x[comando]] [-s <archivo_comandos]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    int status; // la variable status donde se almacenara el estado de terminacion del hijo
    pid_t pid; 

    if(execute_flag){ // OPCION -X
        // ejecutar el comando directamente(opcion -x)
        if(optind < argc){ // comprobamos que hay un comando a continuacion
            char **command_argv = parse_command(argv[optind], &argc); // llamamos a la funcion parse para analizar el comando
            pid = launch_command(command_argv); // ejecutamos el comando y guardamos el pid del hijo en la variabe pid
            waitpid(pid, &status, 0); // &status para apuntar a la dir de la variable status, y 0 porque la funcion es asi

            for(int i=0; command_argv[i] != NULL; i++){
                free(command_argv[i]); // liberamos memoria de cada uno de los argumentos individuales
            }
            free(command_argv); // liberamos la memoria reservada para el array de argumentos
        }
        else{ // si no hay comando detras del -x error
            fprintf(stderr, "Error: La opción -x requiere un comando\n");
            exit(EXIT_FAILURE);
        }
    }else if(command_file != NULL){ // OPCION -S

        FILE *fp = fopen(command_file, "r"); // abrimos el archivo que se pasa como argumento
        if(fp == NULL){
            perror("Error al abrir el archivo de comandos"); 
            exit(EXIT_FAILURE); 
        }

        char line[1024]; // establecemos el tam maximo de la linea
        while(fgets(line, sizeof(line), fp) != NULL){
            
            size_t len = strlen(line); 
            if(len > 0 && line[len -1] == '\n'){ // Esto es para eliminar el salto de linea
                line[len - 1] = '\0'; 
            }

            char **command_argv = parse_command(line, &argc);
            if(command_argv != NULL && command_argv[0] != NULL){ // aseguramos que la linea no esta vacia
                pid = launch_command(command_argv); 
                waitpid(pid, &status, 0);
            }

            // Liberar memoria
            for(int i=0; command_argv[i] != NULL; i++){
                free(command_argv[i]);
            }
            free(command_argv);
        }

        if(ferror(fp)){
            perror("fgets"); 
            exit(EXIT_FAILURE);
        }

        fclose(fp); 
    }else{
        // No se especificó ni -x ni -s
        fprintf(stderr, "Uso: %s [-x] [-s <archivo_comandos>] [comando]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    return 0; 
}