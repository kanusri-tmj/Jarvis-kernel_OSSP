#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>

#define MAX_CHILDREN 100

typedef enum
{
    PENDING,
    STARTED,
    PROCESSING,
    TERMINATING,
    TERMINATED
} ProcessState;

typedef struct
{
    pid_t pid;
    int child_no;
    ProcessState state;
    int exit_status;
    int signal_no;
} ProcessInfo;

ProcessInfo *info;
int child_count;

const char *state_name(ProcessState state)
{
    switch(state)
    {
        case PENDING:
            return "PENDING";

        case STARTED:
            return "STARTED";

        case PROCESSING:
            return "PROCESSING";

        case TERMINATING:
            return "TERMINATING";

        case TERMINATED:
            return "TERMINATED";

        default:
            return "UNKNOWN";
    }
}

int find_index_by_pid(pid_t pid)
{
    for(int i = 0; i < child_count; i++)
    {
        if(info[i].pid == pid)
            return i;
    }

    return -1;
}

void show_process(ProcessInfo *p)
{
    printf("\nChild-%d\n", p->child_no);
    printf("PID : %d\n", p->pid);
    printf("Status : %s\n", state_name(p->state));

    if(p->state == TERMINATED)
    {
        if(p->signal_no != 0)
        {
            printf("Termination : ABNORMAL\n");
            printf("Killed By Signal : %d\n", p->signal_no);
        }
        else
        {
            printf("Termination : NORMAL\n");
            printf("Exit Status : %d\n", p->exit_status);
        }
    }
}

void reap_children(void)
{
    int status;
    pid_t pid;

    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        int index = find_index_by_pid(pid);

        if(index == -1)
            continue;

        info[index].state = TERMINATED;

        if(WIFEXITED(status))
        {
            info[index].exit_status = WEXITSTATUS(status);
            info[index].signal_no = 0;
        }
        else if(WIFSIGNALED(status))
        {
            info[index].signal_no = WTERMSIG(status);
            info[index].exit_status = 0;
        }
    }
}

void query_pid(void)
{
    pid_t pid;

    printf("\nEnter PID number: ");

    if(scanf("%d", &pid) != 1)
    {
        while(getchar() != '\n');

        printf("Invalid PID.\n");
        return;
    }

    reap_children();

    int index = find_index_by_pid(pid);

    if(index == -1)
    {
        printf("PID %d is not a child process of this system.\n", pid);
        return;
    }

    show_process(&info[index]);
}

void show_all(void)
{
    reap_children();

    printf("\n================ PROCESS STATUS TABLE ================\n");

    printf("%-10s %-10s %-15s\n",
           "Child",
           "PID",
           "Status");

    printf("-------------------------------------------------------\n");

    for(int i = 0; i < child_count; i++)
    {
        printf("%-10d %-10d %-15s\n",
               i + 1,
               info[i].pid,
               state_name(info[i].state));
    }

    printf("=======================================================\n");
}

void terminate_child(void)
{
    pid_t pid;

    printf("\nEnter PID to terminate: ");

    if(scanf("%d", &pid) != 1)
    {
        while(getchar() != '\n');

        printf("Invalid PID.\n");
        return;
    }

    reap_children();

    int index = find_index_by_pid(pid);

    if(index == -1)
    {
        printf("PID %d is not a child process of this system.\n", pid);
        return;
    }

    if(info[index].state == TERMINATED)
    {
        printf("PID %d is already terminated.\n", pid);
        return;
    }

    info[index].state = TERMINATING;

    printf("Parent sending SIGKILL to Child-%d (PID=%d)\n",
           info[index].child_no,
           pid);

    if(kill(pid, SIGKILL) == -1)
        perror("kill");
}

int main(void)
{
    printf("=========================================\n");
    printf("PROCESS TERMINATION MESSAGE SYSTEM\n");
    printf("Parent PID : %d\n", getpid());
    printf("=========================================\n\n");

    printf("Enter number of child processes (3-%d): ",
           MAX_CHILDREN);

    if(scanf("%d", &child_count) != 1 ||
       child_count < 3 ||
       child_count > MAX_CHILDREN)
    {
        printf("Invalid number. Please enter a value from 3 to %d.\n",
               MAX_CHILDREN);

        return 1;
    }

    info = mmap(NULL,
                sizeof(ProcessInfo) * child_count,
                PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS,
                -1,
                0);

    if(info == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    for(int i = 0; i < child_count; i++)
    {
        info[i].pid = 0;
        info[i].child_no = i + 1;
        info[i].state = PENDING;
        info[i].exit_status = 0;
        info[i].signal_no = 0;
    }

    printf("\nScheduling and creating %d child processes...\n\n",
           child_count);

    for(int i = 0; i < child_count; i++)
    {
        pid_t pid = fork();

        if(pid < 0)
        {
            perror("fork");
            return 1;
        }

        if(pid == 0)
        {
            info[i].pid = getpid();

            info[i].state = STARTED;

            printf("[Child-%d] PID=%d started\n",
                   i + 1,
                   getpid());

            fflush(stdout);

            info[i].state = PROCESSING;

            int work_time = 5 + (i % 3);

            for(int t = 1; t <= work_time; t++)
            {
                printf("[Child-%d] Processing... %d/%d\n",
                       i + 1,
                       t,
                       work_time);

                fflush(stdout);

                sleep(1);
            }

            info[i].state = TERMINATING;

            printf("[Child-%d] Completing task normally\n",
                   i + 1);

            fflush(stdout);

            exit(10 * (i + 1));
        }

        info[i].pid = pid;
    }

    sleep(1);

    if(child_count >= 3)
    {
        info[2].state = TERMINATING;

        printf("\nParent sending SIGKILL to Child-3 (PID=%d)\n",
               info[2].pid);

        fflush(stdout);

        kill(info[2].pid, SIGKILL);
    }

    int choice;
    int terminated_count = 0;

    while(1)
    {
        reap_children();

        terminated_count = 0;

        for(int i = 0; i < child_count; i++)
        {
            if(info[i].state == TERMINATED)
                terminated_count++;
        }

        printf("\n================ PROCESS MENU ================\n");

        printf("1. Query status using PID\n");
        printf("2. Show all process statuses\n");
        printf("3. Send SIGKILL to a PID\n");
        printf("4. Show termination report\n");
        printf("5. Exit\n");

        printf("==============================================\n");

        printf("Enter choice: ");

        if(scanf("%d", &choice) != 1)
        {
            while(getchar() != '\n');

            printf("Invalid choice.\n");

            continue;
        }

        if(choice == 1)
        {
            query_pid();
        }
        else if(choice == 2)
        {
            show_all();
        }
        else if(choice == 3)
        {
            terminate_child();
        }
        else if(choice == 4)
        {
            reap_children();

            printf("\n=========== TERMINATION REPORT ===========\n");

            for(int i = 0; i < child_count; i++)
            {
                printf("\nChild PID : %d\n",
                       info[i].pid);

                printf("Termination Type : ");

                if(info[i].signal_no != 0)
                {
                    printf("ABNORMAL\n");

                    printf("Killed By Signal : %d\n",
                           info[i].signal_no);
                }
                else if(info[i].state == TERMINATED)
                {
                    printf("NORMAL\n");

                    printf("Exit Status : %d\n",
                           info[i].exit_status);
                }
                else
                {
                    printf("NOT YET TERMINATED\n");
                }
            }
        }
        else if(choice == 5)
        {
            break;
        }
        else
        {
            printf("Invalid choice.\n");
        }

        sleep(1);
    }

    while(wait(NULL) > 0);

    printf("\n=========================================\n");
    printf("All child processes collected.\n");
    printf("No zombie processes exist.\n");
    printf("Project Execution Completed.\n");
    printf("=========================================\n");

    munmap(info,
           sizeof(ProcessInfo) * child_count);

    return 0;
}