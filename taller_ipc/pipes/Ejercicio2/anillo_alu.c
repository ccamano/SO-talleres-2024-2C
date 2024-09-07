#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number() {return (rand() % 50);}

void ejecutar_hijo_inicial(int i, int n, int pipe_fd[][2], 
                           int pipe_enviar_c[2], int pipe_recibir_c[2]) {
  
  close(pipe_enviar_c[PIPE_WRITE]);
  close(pipe_recibir_c[PIPE_READ]);
  
  for (int j = 0; j < n; j++) {
    if (j == ((i - 1) % n)) {
      close(pipe_fd[j][PIPE_WRITE]);
    } else if (i == j) {   
      close(pipe_fd[j][PIPE_READ]);
    } else {
      close(pipe_fd[j][PIPE_READ]);
      close(pipe_fd[j][PIPE_WRITE]);
    }
  }
  
  int c;
  read(pipe_enviar_c[PIPE_READ], &c, sizeof(c));
  int secreto = generate_random_number();
  while(c > secreto) {
    c++;
    write(pipe_fd[(i-1) % n][PIPE_WRITE], &c, sizeof(c));
    read(pipe_fd[(i-1) % n][PIPE_READ], &c, sizeof(c));
  }
  write(pipe_recibir_c[PIPE_WRITE], &c, sizeof(c));
  exit(EXIT_SUCCESS);
}

void ejecutar_hijo_i(int i, int n, int pipe_fd[][2], 
                     int pipe_enviar_c[2], int pipe_recibir_c[2]) {
  
  close(pipe_enviar_c[PIPE_WRITE]);
  close(pipe_enviar_c[PIPE_READ]);
  close(pipe_recibir_c[PIPE_WRITE]);
  close(pipe_recibir_c[PIPE_READ]);

  for (int j = 0; j < n; j++) {
    if (j == ((i - 1) % n)) {
      close(pipe_fd[j][PIPE_WRITE]);
    } else if (i == j) {   
      close(pipe_fd[j][PIPE_READ]);
    } else {
      close(pipe_fd[j][PIPE_READ]);
      close(pipe_fd[j][PIPE_WRITE]);
    }
  }
  
  int c;
  read(pipe_fd[(i - 1) % n][PIPE_READ], &c, sizeof(c));
  c++;
  write(pipe_fd[i][PIPE_WRITE], &c, sizeof(c));
}

int main(int argc, char **argv) {	
  //Funcion para cargar nueva semilla para el numero aleatorio
  srand(time(NULL));

  int status, pid, n, start, buffer;
  n = atoi(argv[1]);
  buffer = atoi(argv[2]);
  start = atoi(argv[3]);

  if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}    
  printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);
  
  int pipe_enviar_c[2];
  pipe(pipe_enviar_c);
  
  int pipe_recibir_c[2];
  pipe(pipe_recibir_c);
  
  int pipes[n][2];
  for (int i = 0; i < n; i++) { 
    pipe(pipes[i]);
  }
  
  for (int i = 0; i < n; i++) {
    pid = fork();
    if (pid == 0) {
      if (i+1 == start) {
        ejecutar_hijo_inicial(i, n, pipes, pipe_enviar_c, pipe_recibir_c);
        write(pipe_enviar_c[PIPE_WRITE], &buffer, sizeof(buffer));
      } else {
        ejecutar_hijo_i(i, n, pipes, pipe_enviar_c, pipe_recibir_c);
      }
    }
  }
  
  int c_actualizado;
  read(pipe_recibir_c[PIPE_READ], &c_actualizado, sizeof(c_actualizado));
  printf("Resultado total: %d\n", c_actualizado);
  
}
