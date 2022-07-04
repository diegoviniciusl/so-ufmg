#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void cpuBound()
{
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 1000000; j++)
        {
            asm("nop");
        }
    }
}

void sCpu()
{
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 1000000; j++)
        {
            asm("nop");
        }
        user_yield();
    }
}

void ioBound()
{
    for (int i = 0; i < 100; i++)
    {
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc > 2 || argc < 1)
    {
        printf(1, "Invalid arguments\n");
        exit();
    }

    int quantity = atoi(argv[1]) * 3;
    int processesAmounts[3];
    int readyTime[3];
    int sleepTime[3];
    int turnaroundTime[3];

    for (int i = 0; i < 3; i++)
    {
        processesAmounts[i] = 0;
        readyTime[i] = 0;
        sleepTime[i] = 0;
        turnaroundTime[i] = 0;
    }

    for (int i = 0; i < quantity; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            int type = getpid() % 3;
            switch (type)
            {
            case 0:
                cpuBound();
                break;
            case 1:
                sCpu();
                break;
            case 2:
                ioBound();
                break;
            }
            exit();
        }
        else
        {
            processesAmounts[pid % 3]++;
            continue;
        }
    }

    for (int i = 0; i < quantity; i++)
    {
        int retime, rutime, stime;
        int childPid = wait2(&retime, &rutime, &stime);
        char *type = childPid % 3 == 0 ? "CPUBound" : (childPid % 3 == 1 ? "SBound" : "IoBound");

        printf(1, "PID: %d\n Type: %s\n Ready time: %d\n Run time: %d\n Sleep time: %d\n\n", childPid, type, retime, rutime, stime);

        readyTime[childPid % 3] += retime;
        sleepTime[childPid % 3] += stime;
        turnaroundTime[childPid % 3] += retime + rutime + stime;
    }

    for (int j = 0; j < 3; j++)
    {
        char *type = j == 0 ? "CPUBound" : (j == 1 ? "SBound" : "IoBound");
        printf(1, "%s Proccess count: %d\n", type, processesAmounts[j]);
        printf(1, "Average ready time: %d\n", readyTime[j]/processesAmounts[j]);
        printf(1, "Average sleeping time: %d\n",  sleepTime[j]/processesAmounts[j]);
        printf(1, "Average turnaround time: %d\n\n", turnaroundTime[j]/processesAmounts[j]);
    }

    exit();
}
