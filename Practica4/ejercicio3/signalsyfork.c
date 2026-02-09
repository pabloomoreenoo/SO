#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//LIBRERIAS QUE HAY QUE METER
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <signal.h> // LIBRERIA PARA TRABAJAR CON SEÑALES

pid_t child_pid;  // variable global para el proceso hijo

// MANEJADOR DE LA SEÑAL
void sigalrm_handler(int signum){

	if(child_pid > 0){
		kill(child_pid, SIGKILL); 
	}

}

pid_t launch_command(char **argv){

	pid_t pid = fork(); 
	if(pid == -1){
		perror("Fork"); 
		exit(EXIT_FAILURE); 
	}

	if(pid == 0){
		execvp(argv[0], argv); 

		perror("execvp"); 
		exit(EXIT_FAILURE); 
	}

	return pid;
}

int main(int argc, char **argv)  // recibe un comando, crea un hijo que lo ejecuta y 
{                                    // se programa una alarma para que finalize el proceso hijo

	int status; 

	if(argc < 2){
		fprintf(stderr, "Uso: %s <comando> [argumentos...]\n", argv[0]);
        exit(EXIT_FAILURE);
	}

	// 1. inicializar el struct e instalar el manejador SIGNALARM
	struct sigaction sa; 
	sa.sa_handler = sigalrm_handler; 
	sigemptyset(&sa.sa_mask); 
	sa.sa_flags = 0; 

	if(sigaction(SIGALRM, &sa, NULL) == -1){
		perror("Error al usar sigaction"); 
		exit(EXIT_FAILURE); 
	}

	// 2. Creamos el proceso hijo y ejecutamos el comando
	child_pid = launch_command(&argv[1]); // pasamos la dir del segundo elemento

	//3. programo la alarma
	alarm(5); 

	waitpid(child_pid, &status, 0); // esperar a que el hijo termine

	if(WIFEXITED(status)){
		printf("El hijo terminó normalmente con código de salida: %d\n", WEXITSTATUS(status));
	}else if(WIFSIGNALED(status)){
		printf("El hijo terminó por una señal: %d\n", WTERMSIG(status));
	}

	return 0;
}
