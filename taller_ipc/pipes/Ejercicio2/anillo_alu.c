#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number() {return (rand() % 50);}

void ejecutar_hijo_inicial(int i, int n, int pipes_hijos[][2], 
                           int pipes_padre[2]) {


printf("Soy proceso inicial: %i y voy a cerrar pipes.\n", i);
  
close(pipes_padre[PIPE_READ]);
close(pipes_padre[PIPE_WRITE]);

  for (int j = 0; j < n; j++) {
    if( j == (i+1) % n){
      close(pipes_hijos[j][PIPE_READ]);
    } else if (j == i){
      close(pipes_hijos[j][PIPE_WRITE]);
    }else {
      close(pipes_hijos[j][PIPE_READ]);
      close(pipes_hijos[j][PIPE_WRITE]);
    }  
  }
  

  int c = 0;
  read(pipes_hijos[i][PIPE_READ], &c, sizeof(c));
  printf("En proceso inicial el padre escribio c: %i\n", c);
  
  // TODO: si c > secreto, ver como seguir la ronda
  // TODO: donde guardar secreto? guardarlo siquiera?
  int secreto = generate_random_number();
  c++;
  printf("En proceso inicial el secreto es: %i y le escribo c: %i al proceso: %i \n\n", secreto, c, (i+1) % n);

  write(pipes_hijos[(i + 1 ) % n][PIPE_WRITE], &c, sizeof(c));
  read(pipes_hijos[(i-1) % n][PIPE_READ], &c, sizeof(c));
 
  exit(EXIT_SUCCESS);
}

void ejecutar_hijo_i(int i, int n, int pipes_hijos[][2], 
                     int pipe_padre[2]) {

  printf("Soy proceso: %i y voy a cerrar pipes.\n", i);
  close(pipe_padre[PIPE_READ]);
  close(pipe_padre[PIPE_WRITE]);
  

  for (int j = 0; j < n; j++) {
    if(j == i){
      close(pipes_hijos[j][PIPE_WRITE]);
    } else if (j == (i + 1) % n ) {
      close(pipes_hijos[j][PIPE_READ]);
    }else{
      close(pipes_hijos[j][PIPE_READ]);
      close(pipes_hijos[j][PIPE_WRITE]);
    }  
  }
  
  int c = 0;
  read(pipes_hijos[i][PIPE_READ], &c, sizeof(c));
  printf("Soy proceso: %i y leo numero que me escribieron: %i\n",i, c);

  c++;
  printf("Soy proceso: %i y escribo en proceso: %i el num: %i\n\n",i, (i+1) % n, c);
  write(pipes_hijos[(i + 1 ) % n][PIPE_WRITE], &c, sizeof(c));

  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {	
  //Funcion para cargar nueva semilla para el numero aleatorio
  srand(time(NULL));

  // n = cantidad de procesos
  // start = proceso inicial
  // buffer = numero c 
  int status, pid, n, start, buffer;

  n = atoi(argv[1]);
  buffer = atoi(argv[2]);
  start = atoi(argv[3]);

  // Hardcodeado para debuggear
  //n= 3;
  //buffer = 2;
  //start = 1;

  // Comentado para debuggear 
  if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}    
  printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n\n", n, buffer, start);
  
  // Pipes de comunicacion hijos a padre
  int pipe_padre[2];
  pipe(pipe_padre);
  
  // Pipes de comunicacion entre hijos
  int pipes_hijos[n][2];
  for (int i = 0; i < n; i++) { 
    pipe(pipes_hijos[i]);
  }
  
  // Padre le escribe al proceso inicial el numero c
  write(pipes_hijos[start-1][PIPE_WRITE], &buffer, sizeof(buffer));

  for (int i = 0; i < n; i++) {
    //pid = fork();
    if (fork() == 0) {
      if (i+1 == start) {
        ejecutar_hijo_inicial(i, n, pipes_hijos, pipe_padre);
      } else {
        ejecutar_hijo_i(i, n, pipes_hijos, pipe_padre);
      }
    } 
  }
  
  // Espera que terminen todos los procesos hijos
  for( int i = 0; i < n; i++){
    wait(NULL);
  }

  // Padre lee resultado final
    // int resultado = 0;
    // read(pipe_padre[PIPE_READ], &resultado, sizeof(resultado));
    // printf('Resultado final es: %i', resultado);
  printf("final del padre \n");
  
}
