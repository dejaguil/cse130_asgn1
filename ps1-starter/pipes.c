#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define READEND 0
#define WRITEEND 1

int main(int argc,char* argv[]) {  
   if (argc < 3) {
      printf("Usage: pipes a b, where a and b are integers. \n");
      exit(1);
   }
   
   int a = atoi(argv[1]);
   int b = atoi(argv[2]);

   int pipe1[2];
   int pipe2[2];

   if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
       perror("pipe failed");
       exit(1);
   }

   pid_t pid1 = fork();
   if (pid1 == 0) {
       // Child 1
       dup2(pipe1[WRITEEND], STDOUT_FILENO);
       close(pipe1[READEND]);
       close(pipe1[WRITEEND]);
       close(pipe2[READEND]);
       close(pipe2[WRITEEND]);

       execlp("ls", "ls", "/dev", NULL);
       perror("execlp ls failed");
       exit(1);
   }

   pid_t pid2 = fork();
   if (pid2 == 0) {
       // Child 2
       dup2(pipe1[READEND], STDIN_FILENO);
       dup2(pipe2[WRITEEND], STDOUT_FILENO);

       close(pipe1[READEND]);
       close(pipe1[WRITEEND]);
       close(pipe2[READEND]);
       close(pipe2[WRITEEND]);

       execlp("xargs", "xargs", NULL);
       perror("execlp xargs failed");
       exit(1);
   }

   pid_t pid3 = fork();
   if (pid3 == 0) {
       // Child 3
       dup2(pipe2[READEND], STDIN_FILENO);

       close(pipe1[READEND]);
       close(pipe1[WRITEEND]);
       close(pipe2[READEND]);
       close(pipe2[WRITEEND]);

       char range[20];
       snprintf(range, sizeof(range), "%d-%d", a, b);

       execlp("cut", "cut", "-d", " ", "-f", range, NULL);
       perror("execlp cut failed");
       exit(1);
   }

   // Parent
   close(pipe1[READEND]);
   close(pipe1[WRITEEND]);
   close(pipe2[READEND]);
   close(pipe2[WRITEEND]);

   // Wait for all children
   for (int i = 0; i < 3; i++) {
       waitpid(-1, NULL, 0);
   }
   return 0;
}


