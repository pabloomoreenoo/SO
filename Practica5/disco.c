#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <ctype.h>
//LIBRERIAS QUE HAY QUE METER
#include <sys/types.h>
#include <sys/wait.h> 
#include <signal.h> // LIBRERIA PARA TRABAJAR CON SEÑALES
#include <pthread.h> // Para trabajar con hilos

#define CAPACITY 1
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

typedef struct{   // tipo estructura para guardar los datos del cliente
	int id; 
	int isvip;
	int turno; 
}client_args_t; 

// variables globales (mutex, variables de condicion, contador de clientes)

pthread_mutex_t mutex; // para bloquear y desbloquear y evitar que los hilos se desincronicen
// variables de condicion
pthread_cond_t cond_vip;  
pthread_cond_t cond_normal; 

int num_clients_inside = 0; // contador para el numero de clientes que hay dentro de la discoteca
int num_vips_waiting = 0; 

//TURNOS
int turno_normal_actual = 0; 
int turno_vip_actual = 0; 

void enter_normal_client(int id, int turno)
{
	// Bloqueo el mutex
	pthread_mutex_lock(&mutex); 

	// Mientras la discoteca este llena o haya vips esperando:
	        // espero
	while(num_clients_inside >= CAPACITY || num_vips_waiting > 0 || turno != turno_normal_actual){
		//printf("Cliente normal %d en espera por aforo lleno.\n", id); 
		pthread_cond_wait(&cond_normal, &mutex); 
	}

	// Aumento el numero de clientes
	num_clients_inside++; 
	printf("Cliente normal %d ha entrado. Aforo: %d\n", id, num_clients_inside);

	turno_normal_actual++; // pasamos al siguiente turno

	// Desbloqueo el mutex
	pthread_mutex_unlock(&mutex); 
}

void enter_vip_client(int id, int turno)
{
	// Bloqueo el mutex
	pthread_mutex_lock(&mutex); 

	num_vips_waiting++; 
	// Mientras la discoteca este llena:
	        // espero
	while(num_clients_inside >= CAPACITY || turno != turno_vip_actual){
		//printf("Cliente VIP %d en espera por aforo lleno.\n", id); 
		pthread_cond_wait(&cond_vip, &mutex); 
	}
	num_vips_waiting--; 

	// Aumento el numero de clientes
	num_clients_inside++; 
	printf("Cliente VIP %d ha entrado. Aforo: %d\n", id, num_clients_inside); 

	turno_vip_actual++; // Pasamos al siguiente turno vip

	// Desbloqueo el mutex
	pthread_mutex_unlock(&mutex); 
}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	// bloqueo en el mutex
	pthread_mutex_lock(&mutex); 

	// decremento del numero de clientes
	num_clients_inside--; 
	printf("Cliente %s con ID %d ha salido. Aforo actual %d\n", isvip ? "VIP" : "normal", id, num_clients_inside); 

	// Aviso a los clientes que pueden entrar
	if(num_vips_waiting > 0){
		pthread_cond_broadcast(&cond_vip); // si hay vips esperando, llamo a los vips
	}else{
		pthread_cond_broadcast(&cond_normal); // si no llamo a los normales
	}

	// desbloqueo el mutex
	pthread_mutex_unlock(&mutex); 
}

void *client(void *arg)
{
	//obtener el id y isvip del argumento
	client_args_t client = *(client_args_t *)arg; 
	int id = client.id; 
	int isvip = client.isvip; 
	int turno = client.turno; 

	if(isvip){
		enter_vip_client(id, turno); 
	}else{
		enter_normal_client(id, turno);
	}

	dance(id, isvip); 

	disco_exit(id, isvip); 

	pthread_exit(NULL);  // esto nos dice que el hilo ha terminado y a muerto una vez realizada la ejecucion de la funcion anonima
}

int main(int argc, char *argv[])
{
	int turno_cliente_vip = 0; 
	int turno_cliente_normal = 0; 

	// comprobar argumentos
	if(argc != 2){
		perror("Error, debes introducir 2 argumentos"); 
	}

	// abrir fichero
	FILE *file; 
	if((file = fopen(argv[1], "r")) == NULL){
		fprintf(stderr, "file could not be opened: ");
		exit(EXIT_FAILURE); 
	}

	// leer numero de clientes
	int size; // aqui vamos a almacenar el primer valor que se lee, es decir, el numero de clientes totales que hay en el archivo 
	int result = fscanf(file, "%d", &size); // result es el numero de elementos leidos y asignados correctamente
	if(result == -1){
		perror("Error al leer el contenido del archivo"); 
		exit(EXIT_FAILURE); 
	}
	//printf("El numero de clientes es:  %d\n", size); 
	

	// crear array de clients_args_t
	client_args_t clients_array[size]; 

	// crear array de pthread_t
	pthread_t pthread_array[size]; 

	// inicializar mutex y variables de condicion
	pthread_mutex_init(&mutex, NULL); 
	pthread_cond_init(&cond_vip, NULL); 
	pthread_cond_init(&cond_normal, NULL); 

	// bucle para crear hilos: 
	for(int i=0; i<size; i++){

		// leer el tipo de cliente del fichero
		int type; 
		int elem = fscanf(file, "%d", &type);
		//printf("El tipo de cliente es  %d \n", type); 
		if(elem == -1){
			perror("Error al procesar el numero encontrado"); 
			exit(EXIT_FAILURE); 
		} 

		// Rellenar estructura para clients_args_t
		clients_array[i].isvip = type; 
		clients_array[i].id = i; 

		// Asignamos un turno a los clientes
		if(type == 1){
			clients_array[i].turno = turno_cliente_vip; 
			turno_cliente_vip++; 
		}else{
			clients_array[i].turno =  turno_cliente_normal; 
			turno_cliente_normal++; 
		}

		// Crear hilo con pthread_create
		pthread_create(&pthread_array[i], NULL, client, &clients_array[i]); 

		// comprobar errores
	}

	// bucle para esperar a los hilos:
	for(int i=0; i<size; i++){
		// pthread_join
		if(pthread_join(pthread_array[i], NULL) != 0){ // comprobar errores
			perror("Error pthread_join"); 
			exit(EXIT_FAILURE); 
		}
	}

	// Destruir mutex y variables de condicion
	pthread_mutex_destroy(&mutex); 
	pthread_cond_destroy(&cond_vip); 
	pthread_cond_destroy(&cond_normal); 
	
	// cerrar fichero
	if(ferror(file)){
		perror("Error al leer el archivo"); 
		fclose(file); 
		exit(EXIT_FAILURE); 
	}
	fclose(file); 
	
	return 0;
}
