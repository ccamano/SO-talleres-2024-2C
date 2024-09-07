#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number() {return (rand() % 50);}

void ejecutar_hijo_inicial(int c, int pipe[2]) {
  int secreto = generate_random_number();
  while(true)
    if (c > secreto) {
      // TODO: enviar al padre el c
      exit(EXIT_SUCCESS);
    } else {
      c++;
      write(pipe[PIPE_WRITE], &c, sizeof(c));
      read(pipe[PIPE_READ, &c, sizeof(c)]);
    }    
  }
}

void mensajear(int c, int pipe[2]) {
  int valor;
  read(pipe[PIPE_READ], &valor, sizeof(valor));
  valor++;
  write(pipe[PIPE_WRITE], &valor, sizeof(valor));
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
  
  int pipe_recibir_c_actualizado[2];
  pipe(pipe_recibir_c_acutalizado);
  
  int pipes_entre_hijos[n][2];
  for (int i = 0; i < n; i++) { 
    pipe(pipes[i]);
  }
  
  pid_t pid;
  for (int i = 0; i < n; i++) {
    pid = fork();
    if (pid == 0) {
      if (i == start) {
        ejecutar_hijo_inicial(n, pipes)
        write(pipe_enviar_c[PIPE_WRITE], &c, sizeof(c));
      } else {
        ejecutar_hijo(pipes);
      }
    }
  }
  
  int c_actualizado;
  read(pipe_recibir_c_actualizado[PIPE_READ, &c_actualizado, sizeof(c_acutalizado)]);
  printf("Resultado total: %d\n", c_actualizado);
  
}
