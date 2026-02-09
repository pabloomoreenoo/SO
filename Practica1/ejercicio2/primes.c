/**
 *  This program calculates the sum of the first n prime
 *  numbers. Optionally, it allows the user to provide as argument the
 *  value of n, which is 10 by default.
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 * This function takes an array of integers and returns the sum of its n elements.
 */
int sum(int *arr, int n);

/**
 * This function fills an array with the first n prime numbers.
 */
void compute_primes(int* result, int n);

/**
 * This function returns 1 if the integer provided is a prime, 0 otherwise.
 */
int is_prime(int x);

int main(int argc, char **argv) {

  int n = 10; // by default the first 10 primes

  if(argc == 2) {
    n = atoi(argv[1]);  // utilizamos la funcion atoi() para hacer el cast 
  }
  int* primes = (int*)malloc(n*sizeof(int));  // reservamos memoria para las n posiciones del array
  compute_primes(primes, n); // llamamos a la funcion compute_primes

  int s = sum(primes, n); 
  printf("The sum of the first %d primes is %d\n", n, s);

  free(primes); // liberamos el espacio en memoria al acabar
  return 0;
}

int sum(int *arr, int n) {  // esta funcion la hemos tenido que corregir 
  int i;
  int total = 0;
  for(i=0; i<n; i++) {
    total += arr[i];
  }
  return total;
}

void compute_primes(int* result, int n) {
  int i = 0;
  int x = 0;
  while(i < n) {  // hemos tenido que modificar el bucle de esta funcion para que no sea infinito
    if(is_prime(x)) {
      result[i] = x; 
      i++; 
    }
    x++; 
  }
  return;
}

int is_prime(int x) {  // hemos modificado esta funcion para que sea correcta
  if(x < 2){
    return 0; 
  }

  if(x == 2){
    return 1;
  }

  if(x % 2 == 0) {
    return 0;
  }

  for(int i=3; i<x; i+=2) {
    if(x % i == 0) {
      return 0;
    }
  }
}
