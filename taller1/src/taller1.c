#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

int n;
int j;
int k;
pid_t childArr[];

void handler_ruleta(int sig) {
  int random = rand() % n + 1;
  printf("Recibi el número %d\n", random);
  if (random == j) { // Si saqué el número maldito
    printf("Saque numero maldito \n");
    exit(EXIT_SUCCESS);
  }
}

void handler_murio_hijo(int sig) {
  int child_pid = wait(NULL); 
  for (int i = 0; i < n; i++) { 
    if (childArr[i] == child_pid) childArr[i] = -1;
  }
}

void survivorsPrint(pid_t *childArray, int k, int n) {
  printf("Sobrevivientes luego de %d rondas:\n", k);
  printf("=================================\n");
  for (int i = 0; i < n; i++) {
    if (childArray[i] != -1) {
      printf("Soy el proceso %d, hijo nro %d y sobrevivi.\n", childArray[i], i);
    }
  }
}

int main(int argc, char *argv[]) {
  n = atoi(argv[1]); // n = cant procesos < 10
  k = atoi(argv[2]); // k = cant rondas
  j = atoi(argv[3]); // j = num maldito (0 <= j < n)

  if (n >= 10) {
    printf("Numero erroneo de procesos\n");
    return 1;
  } else if (j < 0 || j >= n) {
    printf("Numero maldito erroneo\n");
    return 1;
  }
  
  
  
  pid_t currentPid = 1;

  // Creamos los n hijos, sabiendo que estamos en el proceso padre
  for (int i = 0; i < n; i++) {
    currentPid = fork();
    if (currentPid != 0) { 
      sleep(1);
      childArr[i] = currentPid;
      printf("Hijo " "%d" " creado con pid " "%d" "\n", i+1, currentPid);
    } else {
      srand(time(NULL));
      signal(SIGTERM, handler_ruleta);
      while (1) {}
      return 0;
    }
  }
  
  printf("Fin creacion de los procesos hijos\n");
  
  signal(SIGCHLD, handler_murio_hijo);
  
  for (int i = 0; i < k; i++) {
    for (int j = 0; j < n; j++) {
      if (childArr[j] != -1) {
        kill(childArr[j], SIGTERM); // Padre envía senal sigterm a los hijos
        sleep(1);                   // Y espera 1 segundo
      }
    }
    
    survivorsPrint(childArr, k, n);
  }
  return 0;
}
