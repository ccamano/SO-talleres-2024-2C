#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

typedef char*** matrix;

void ejecutar_cmd(char* cmd, char** fila) {
  execvp(cmd, fila);
}

/*
	Explicacion del argumento progs:

	Corresponde con una matriz donde cada fila representa el iesimo programa pasado
	dividido por el pipe. En cada fila esta el nombre del programa 
	en su primera posicion y los parametros en las posiciones restantes.

	Ejemplo: 'du -h /home | head -n 10 | sort -rh'
	
	Representacion:
	[
		['du', '-h', '/home'],
		['head', '-n', '10'],
		['sort', '-rh']
	]

*/

void ejecutar_hijo_1(int count, matrix progs, int pipe_fd[][2]) {
  
  close(pipe_fd[0][PIPE_READ]);
  // Conectar escritura a stdout
  dup2(pipe_fd[0][PIPE_WRITE], STD_OUTPUT);
  // Cerrar lectura
  for (int i = 1; i < count; i++) {
    close(pipe_fd[i][PIPE_READ]);
    close(pipe_fd[i][PIPE_WRITE]);
  }
  // Ejecutar programa
  ejecutar_cmd(progs[0][0], progs[0]);
}
        
void ejecutar_hijo_i(int count, matrix progs, int i, int pipe_fd[][2]) {

  // Conectar lectura a stdin
  dup2(pipe_fd[i-1][PIPE_READ], STD_INPUT);
  // Conectar escritura a stdout
  dup2(pipe_fd[i][PIPE_WRITE], STD_OUTPUT);
  
  for (int j = 0; j < count; j++) {
    if (j == i - 1) {
      close(pipe_fd[j][PIPE_WRITE]);
    } else if (i == j) {   
      close(pipe_fd[j][PIPE_READ]);
    } else {
      close(pipe_fd[j][PIPE_READ]);
      close(pipe_fd[j][PIPE_WRITE]);
    }
  }

  // Ejecutar programa
  ejecutar_cmd(progs[i][0], progs[i]);
} 

void ejecutar_hijo_n(int count, matrix progs, int pipe_fd[][2]) {
  // Conectar lectura a stdin
  close(pipe_fd[count - 1][PIPE_WRITE]);
  dup2(pipe_fd[count - 1][PIPE_READ], STD_INPUT);
  // Cerrar escritura
  for (int j = 0; j < count-1; j++) {
      close(pipe_fd[j][PIPE_READ]);
      close(pipe_fd[j][PIPE_WRITE]);
  }
  // Ejecutar programa
  ejecutar_cmd(progs[count][0], progs[count]);
}

void ejecutar_hijo(matrix progs, int i, int count, int pipe[][2]) {
  if (i == 0) {
    ejecutar_hijo_1(count, progs, pipe);
  } else if (i != count && i != 0) {
    ejecutar_hijo_i(count, progs, i, pipe);
  } else {
    ejecutar_hijo_n(count, progs, pipe);
  }
}

static int run(matrix progs, size_t count)
{	
	int r, status;

	
	int pipes[count - 1][2];
	for (int i = 0; i < count - 1; i++) { 
	  pipe(pipes[i]);
	}
	
	//Reservo memoria para el arreglo de pids
	pid_t *children = malloc(sizeof(*children) * count);
	
	pid_t pid;
	for (int i = 0; i < count; i++) {
	  pid = fork();
	  if (pid != 0) {
	    children[i] = pid;
	  } else {
	    ejecutar_hijo(progs, i, count - 1, pipes);
	  }
	}
	
        //TODO: Para cada proceso hijo:
	//1. Redireccionar los file descriptors adecuados al proceso
	//2. Ejecutar el programa correspondiente

        for (int i = 0; i < count - 1; i++) {  
          close(pipes[i][PIPE_WRITE]);
          close(pipes[i][PIPE_READ]);
        }

	//Espero a los hijos y verifico el estado que terminaron
	for (int i = 0; i < count; i++) {
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status)) {
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
			    (int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;
	free(children);

	return r;
}


int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n"); 
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}
	int programs_count;
	matrix programs_with_parameters = parse_input(argv, &programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}
