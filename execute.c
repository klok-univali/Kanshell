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
void printTime(struct timeval, struct timeval, struct timespec, long int, long int);
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
  struct timeval cpu, user;
  struct timespec clock;
  long int interruption_v, interruption_i;

  system(command);
   
  /* Get time */
  getTime(&cpu, &user, &interruption_v, &interruption_i);
  clock_gettime(CLOCK_REALTIME, &clock);

  /* Calculate and print time */
  printTime(cpu, user, clock, interruption_v, interruption_i);
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
    }else if(strncmp(command, "clear", 5) == 0){
      clearScreen();
      continue;
    }

    if((pid = fork()) < 0){
      perror("fork");
      break;
    }

    if(pid == 0){
      if(strncmp(command, "cd ", 3) == 0){
        memcpy(command, &command[3], N);	
        command[(strlen(command)-1)] = '\0';

        if(chdir(command) != 0){
          printf("Directory not found!\n");
        }
        continue;
      }

      executeCommand(command);
      _exit(0);
    }else{
      wait(NULL);
      strcpy(command, "");
    }
  }
}


/**
 *
 */
void getTime(struct timeval *cpu, struct timeval *user, long int *interruption_v, long int *interruption_i){
  struct rusage usage;
  getrusage(RUSAGE_CHILDREN, &usage);

  *cpu = usage.ru_stime;
  *user = usage.ru_utime;
  *interruption_v = usage.ru_nvcsw;
  *interruption_i = usage.ru_nivcsw;
}


/**
 *
 */
void printTime(struct timeval cpu, struct timeval user, struct timespec clock, long int interruption_v, long int interruption_i){
  printf("\n");
  printf("time CPU: %ld.%lds\n", cpu.tv_sec, cpu.tv_usec);
  printf("time USER: %ld.%lds\n", user.tv_sec, user.tv_usec);
  //printf("wall time: %fs\n", ((double)(clock.tv_sec+clock.tv_nsec*1e-9)));
  printf("wall time: %ld.%lds\n", clock.tv_sec, clock.tv_nsec);
  printf("\n");
  printf("voluntary interruption: %li\n", interruption_v);
  printf("involuntary interruption: %li\n", interruption_i);
}


/**
 *
 */
void clearScreen(){
  system("clear");
  printf("Kanshell console\n");
}
