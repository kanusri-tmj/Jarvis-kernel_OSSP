#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
 
int main()
{
pid_t child1, child2, child3;
pid_t terminated_pid;
int status;
 
printf("=========================================\n");
printf("PROCESS TERMINATION MESSAGE SYSTEM\n");
printf("Parent PID : %d\n", getpid());
printf("=========================================\n\n");
 
/* Child 1 : Normal Termination */
 
child1 = fork();
 
if(child1 == 0)
{
printf("[Child-1] PID=%d started\n", getpid());
 
sleep(2);
 
printf("[Child-1] Completing task normally\n");
 
exit(10);
}
 
/* Child 2 : Normal Termination */
 
child2 = fork();
 
if(child2 == 0)
{
printf("[Child-2] PID=%d started\n", getpid());
 
sleep(4);
 
printf("[Child-2] Completing task normally\n");
 
exit(20);
}
 
/* Child 3 : Abnormal Termination */
 
child3 = fork();
 
if(child3 == 0)
{
printf("[Child-3] PID=%d started\n", getpid());
 
while(1)
{
printf("[Child-3] Running...\n");
sleep(1);
}
}
 
sleep(3);
 
printf("\nParent sending SIGKILL to Child-3 (PID=%d)\n\n",
child3);
 
kill(child3, SIGKILL);
 
printf("=========== TERMINATION REPORT ===========\n");
 
while((terminated_pid = wait(&status)) > 0)
{
printf("\nChild PID : %d\n", terminated_pid);
 
if(WIFEXITED(status))
{
printf("Termination Type : NORMAL\n");
printf("Exit Status : %d\n",
WEXITSTATUS(status));
}
else if(WIFSIGNALED(status))
{
printf("Termination Type : ABNORMAL\n");
printf("Killed By Signal : %d\n",
WTERMSIG(status));
}
}
 
printf("\n=========================================\n");
printf("All child processes collected.\n");
printf("No zombie processes exist.\n");
printf("Project Execution Completed.\n");
printf("=========================================\n");
 
return 0;
}