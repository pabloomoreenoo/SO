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

// ENTENDEMOS BIEN EL FUNCIONAMIENTO DE LOS HILOS
/*     
void *funcion_hilo(void *arg){
    int id = *(int *)arg; // Castear void* a int*
    printf("Hola desde el hilo %d\n", id);
    sleep(1);
    printf("Hilo %d terminando...\n",id);
    pthread_exit(NULL); // Terminar el hilo
}

int main(){
    pthread_t hilo1, hilo2; 
    int id1 = 1, id2 = 2; 

    //crear los hilos
    if(pthread_create(&hilo1, NULL, funcion_hilo, &id1) != 0){
        perror("Error al crear el hilo 1"); 
        exit(EXIT_FAILURE); 
    }
    if(pthread_create(&hilo2, NULL, funcion_hilo, &id2) != 0){
        perror("Error al crear el hilo 2"); 
        exit(EXIT_FAILURE); 
    }

     // Esperar a que los hilos terminen
     pthread_join(hilo1, NULL);
     pthread_join(hilo2, NULL);
 
     printf("Todos los hilos han terminado.\n");
     return 0;
}*/

/*// VEMOS COMO FUNCIONAN LOS MUTEX
int contador = 0; // Variable global (recurso compartido)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para proteger el contador

void *incrementar(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&mutex); // Adquirir el mutex
        contador++; // Sección crítica
        pthread_mutex_unlock(&mutex); // Liberar el mutex
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t hilo1, hilo2;

    pthread_create(&hilo1, NULL, incrementar, NULL);
    pthread_create(&hilo2, NULL, incrementar, NULL);

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    printf("Contador: %d\n", contador); // Debería ser 2000000
    return 0;
}*/

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
int count = 0; // Número de elementos en el buffer
int in = 0;  // Índice para insertar
int out = 0; // Índice para extraer

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_lleno = PTHREAD_COND_INITIALIZER; // Condición: buffer lleno
pthread_cond_t cond_vacio = PTHREAD_COND_INITIALIZER; // Condición: buffer vacío

void *productor(void *arg) {
    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&mutex); // Adquirir el mutex

        // Esperar si el buffer está lleno
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&cond_lleno, &mutex); // Esperar, liberando el mutex
        }

        // Producir un elemento (añadir al buffer)
        buffer[in] = i;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("Productor: insertó %d, count = %d\n", i, count);

        pthread_cond_signal(&cond_vacio); // Señalizar que el buffer ya no está vacío
        pthread_mutex_unlock(&mutex); // Liberar el mutex
        //usleep(rand()%100000); //Opcional, para simular trabajo
    }
    pthread_exit(NULL);
}

void *consumidor(void *arg) {
    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&mutex); // Adquirir el mutex

        // Esperar si el buffer está vacío
        while (count == 0) {
            pthread_cond_wait(&cond_vacio, &mutex); // Esperar, liberando el mutex
        }

        // Consumir un elemento (sacar del buffer)
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("Consumidor: extrajo %d, count = %d\n", item, count);

        pthread_cond_signal(&cond_lleno); // Señalizar que el buffer ya no está lleno
        pthread_mutex_unlock(&mutex); // Liberar el mutex
         //usleep(rand()%100000); //Opcional, para simular trabajo.
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t prod_hilo, cons_hilo;

    pthread_create(&prod_hilo, NULL, productor, NULL);
    pthread_create(&cons_hilo, NULL, consumidor, NULL);

    pthread_join(prod_hilo, NULL);
    pthread_join(cons_hilo, NULL);

    return 0;
}