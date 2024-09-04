#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

typedef char*** matrix;

void ejecutar_cmd(char* cmd, char* fila) {
  execlp(cmd, fila, NULL);
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
  //printf("tengo %d filas y en esta fila %d columnas\n", 
  //            sizeof(pipe_fd)/sizeof(pipe_fd[0]),
  //            sizeof(pipe_fd[0])/sizeof(pipe_fd[0][1]));
  // Cerrar lectura
  close(pipe_fd[0][PIPE_READ]);
  for (int i = 1; i < count; i++) {
    close(pipe_fd[i][PIPE_READ]);
    close(pipe_fd[i][PIPE_WRITE]);
  }
  // Conectar escritura a stdout
  dup2(pipe_fd[0][PIPE_WRITE], STD_OUTPUT);
  // Ejecutar programa
  ejecutar_cmd(progs[0][0], progs[0]);
}
        
void ejecutar_hijo_i(int count, matrix progs, int i, int pipe_fd[][2]) {

  for (int j = 1; j < count; j++) {
    if (i != j) {
      close(pipe_fd[j][PIPE_READ]);
      close(pipe_fd[j][PIPE_WRITE]);
    }
  }

  // Conectar lectura a stdin
  dup2(pipe_fd[i][PIPE_READ], STD_INPUT);
  // Conectar escritura a stdout
  dup2(pipe_fd[i][PIPE_WRITE], STD_OUTPUT);
  // Ejecutar programa
  ejecutar_cmd(progs[i][0], progs[i]);
} 

void ejecutar_hijo_n(int count, matrix progs, int pipe_fd[][2]) {
  // Cerrar escritura
  // printf("estoy x cerrar en el ultimo hijo\n");
  close(pipe_fd[count - 1][PIPE_WRITE]);
  for (int j = 0; j < count-1; j++) {
      close(pipe_fd[j][PIPE_READ]);
      close(pipe_fd[j][PIPE_WRITE]);
  }
  // Conectar lectura a stdin
  dup2(pipe_fd[count - 1][PIPE_READ], STD_INPUT);
  // Ejecutar programa
  ejecutar_cmd(progs[count - 1][0], progs[count - 1]);
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

	//Reservo memoria para el arreglo de pids
	//TODO: Guardar el PID de cada proceso hijo creado en children[i]
	///////////////////////
	
	int pipes[count - 1][2];
	for (int i = 0; i < count - 1; i++) { 
	  pipe(pipes[i]);
	}
	
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
	
		

	
	//TODO: Pensar cuantos procesos necesito ///////////////       N
	//TODO: Pensar cuantos pipes necesito. /////////////////       N - 1

	//TODO: Para cada proceso hijo:
	      //1. Redireccionar los file descriptors adecuados al proceso
	      //2. Ejecutar el programa correspondiente

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

        close(pipes[count - 1][PIPE_WRITE]);
        for (int i = 0; i < count - 1; i++) {  
          close(pipes[i][PIPE_WRITE]);
          close(pipes[i][PIPE_READ]);
        }

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
