#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 100

char *getcwd(char *buf, size_t size);

/**
 *
 * Software for simulate terminal shell. ( Computer Science - Operation System )
 *
 * @author Gabriel H. Klok
 * @author Maykon Anschau Oliveira
 * 
 * @since 2018-04-05
 * @version 1.0
 *
 */

void executeCommand(char*);
void openConsoleShell();
void getTime(struct timeval*, struct timeval*, long int *, long int *);
void printTime(struct timeval, struct timeval, struct timeval, struct timeval, struct timespec, struct timespec, long int, long int, long int, long int);
void clearScreen();

/**
 *
 */
int main(int argc, char** argv[]){
  char command[N];
 
  if(argc > 1){
    strcpy(command, "");

    for(int i = 1; i < argc; i++){
      strcat(command, argv[i]);
      strcat(command, " ");
    }
   
    executeCommand(command);
  }else{
    openConsoleShell();
  }

  return(1);
}


/**
 *
 */
void executeCommand(char* command){
  struct timeval cpu_s, cpu_e, user_s, user_e;
  struct timespec clock_s, clock_e;
  long int interruption_vs, interruption_ve, interruption_is, interruption_ie;

  /* Get start time */
  getTime(&cpu_s, &user_s, &interruption_vs, &interruption_is);
  clock_gettime(CLOCK_REALTIME, &clock_s);

  system(command);
   
  /* Get end time */
  getTime(&cpu_e, &user_e, &interruption_ve, &interruption_ie);
  clock_gettime(CLOCK_REALTIME, &clock_e);

  /* Calculate and print time */
  printTime(cpu_s, cpu_e, user_s, user_e, clock_s, clock_e, interruption_vs, interruption_ve, interruption_is, interruption_ie);
}


/**
 *
 */
void openConsoleShell(){
  char command[N]; 
  char cwd[1024];
  pid_t pid;

  clearScreen();

  while(1){
    printf("%% ");
    fgets(command, N, stdin);

    if(strncmp(command, "exit", 4) == 0){
      break;
    }

    if((pid = fork()) < 0){
      perror("fork");
      break;
    }

    if(pid == 0){
      if(strncmp(command, "clear", 5) == 0){
        clearScreen();
        continue;
      }else if(strncmp(command, "cd ", 3) == 0){
        memcpy(command, &command[3], N);	
        command[(strlen(command)-1)] = '\0';

        if(chdir(command) != 0){
          printf("Directory not found!\n");
        }
        continue;
      }

      executeCommand(command);
      strcpy(command, "");
      _exit(0);
    }else{
      wait(NULL);
    }
  }
}


/**
 *
 */
void getTime(struct timeval *cpu, struct timeval *user, long int *interruption_v, long int *interruption_i){
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);

  *cpu = usage.ru_stime;
  *user = usage.ru_utime;
  *interruption_v = usage.ru_nvcsw;
  *interruption_i = usage.ru_nivcsw;
}


/**
 *
 */
void printTime(struct timeval cpu_s, struct timeval cpu_e, struct timeval user_s, struct timeval user_e, struct timespec clock_s, struct timespec clock_e, long int interruption_vs, long int interruption_ve, long int interruption_is, long int interruption_ie){
  printf("\n");
  printf("time CPU: %ld.%lds\n", (cpu_e.tv_sec - cpu_s.tv_sec), (cpu_e.tv_usec - cpu_s.tv_usec));
  printf("time USER: %ld.%lds\n", (user_e.tv_sec - user_s.tv_sec), (user_e.tv_usec - user_s.tv_usec));
  printf("wall time: %fs\n", ((double)((clock_e.tv_sec+clock_e.tv_nsec*1e-9)-(double)(clock_s.tv_sec+clock_s.tv_nsec*1e-9))));
  printf("\n");
  printf("voluntary interruption: %li\n", (interruption_ve - interruption_vs));
  printf("involuntary interruption: %li\n", (interruption_ie - interruption_is));
}


/**
 *
 */
void clearScreen(){
  system("clear");
  printf("Kanshell console\n");
}
