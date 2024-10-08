#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number() {return (rand() % 50);}

void cerrar_pipes(int n, int i, int pipes_hijos[][2]) {
  for (int j = 0; j < n; j++){
    if (j == ((n + i - 1) % n)){
      close(pipes_hijos[j][PIPE_WRITE]);
    } else if (i == j) {
      close(pipes_hijos[j][PIPE_READ]);
    } else {
      close(pipes_hijos[j][PIPE_READ]);
      close(pipes_hijos[j][PIPE_WRITE]);
    }
  }
}

void ejecutar_hijo_inicial(int i, int n, int pipes_hijos[][2], int pipe_padre[2]) {
  
  cerrar_pipes(n, i, pipes_hijos);

  int c;
  read(pipe_padre[PIPE_READ], &c, sizeof(c));
  
  int secreto = generate_random_number();
  secreto += c;
  printf("número secreto %d\n", secreto);
  
  while(c < secreto) {
    c++;
    write(pipes_hijos[i][PIPE_WRITE], &c, sizeof(c));
    read(pipes_hijos[(n + i - 1) % n][PIPE_READ], &c, sizeof(c));
    printf("recibi el c con valor: %d en hijo: %d\n", c, i);
  }
  
  write(pipe_padre[PIPE_WRITE], &c, sizeof(c));
  exit(EXIT_SUCCESS);

}

void ejecutar_hijo_i(int i, int n, int pipes_hijos[][2]) {

  cerrar_pipes(n, i, pipes_hijos);

  int c;
  while (1) {
    read(pipes_hijos[(n + i - 1) % n][PIPE_READ], &c, sizeof(c));
    printf("recibi el c con valor: %d en hijo: %d\n", c, i);
    c++;
    write(pipes_hijos[i][PIPE_WRITE], &c, sizeof(c));
  }

}


int main(int argc, char **argv)
{
  //Funcion para cargar nueva semilla para el numero aleatorio
  srand(time(NULL));

  int status, pid, n, start, buffer;
  n = atoi(argv[1]);
  buffer = atoi(argv[2]);
  start = atoi(argv[3]);

  if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}    
  printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n\n", n, buffer, start);
  
  int pipe_padre[2];
  pipe(pipe_padre);
  
  int pipes_hijos[n][2];
  for (int i = 0; i < n; i++) { 
    pipe(pipes_hijos[i]);
  }
  
  pid_t *children = malloc(sizeof(*children) * n);
  write(pipe_padre[PIPE_WRITE], &buffer, sizeof(buffer)); 

  for (int i = 0; i < n; i++) {
    if ((pid = fork()) == 0) {
      if (i+1 == start) {
        ejecutar_hijo_inicial(i, n, pipes_hijos, pipe_padre);
      } else {
        close(pipe_padre[PIPE_WRITE]);
        close(pipe_padre[PIPE_READ]);

        ejecutar_hijo_i(i, n, pipes_hijos);
      }
    } else {
      children[i] = pid;
    }
  }
    
  int resultado;
  read(pipe_padre[PIPE_READ], &resultado, sizeof(resultado));  
  printf("Resultado total: %d\n", resultado);
  
  for (int i = 0; i < n; i++) {
    if (i+1 != start) kill(children[i], SIGKILL);  
  }
  
  return 0;
}
