#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int stdout = 1;
int multiplicador = 3;

void
CPU_Bound()
{
    for (int x = 0; x < 100; x++){
        for (int y = 0; y < 1000000; y++){
            asm("nop");
        }
    }
}

void
S_Bound()
{
    for (int x = 0; x < 100; x++){
        for (int y = 0; y < 1000000; y++){
            asm("nop");
        }
        user_yield();
    }
}

void
IO_Bound(){
    for (int x = 0; x < 100; x++){
        sleep(1);
    }
}

char*
get_type(int i)
{
    if (i == 0)
    {
        return "CPU-Bound";
    }
    else if (i == 1)
    {
        return "S-Bound";
    }
    else
    {
        return "IO-Bound";
    }
}

int
main(int argc, char* argv[]) 
{
    int n = atoi(argv[1]);
    if (n <= 0) {
        printf(stdout, "ERRO!: sanity <n>, n deve ser > 1\n");
        exit();
    }

    int p[3], ready[3], sleeping[3], turnaround[3];

    // Inicialização
    for (int i = 0; i < 3; i++){
        p[i] = 0;
        ready[i] = 0;
        sleeping[i] = 0;
        turnaround[i] = 0;
    }

    for (int i = 0; i < multiplicador * n; i++){

        int pid = fork();

        if (pid == 0){

            if (getpid()%3 == 0){
                CPU_Bound();
            }

            if (getpid()%3 == 1){
                S_Bound();

            }

            if (getpid()%3 == 2){
                IO_Bound();
            }
            
            exit();
        } 

        else{
            p[pid % 3]++;
            continue;
        }
    }

    for (int x = 0; x < multiplicador * n; x++){

        int retime = 0, rutime = 0, stime = 0;
        int pid_child = wait2(&retime, &rutime, &stime);
        int index = pid_child % 3;
        char *type = get_type(index);
        
        printf(stdout, "> PID do processo = %d\n / Tipo = %s\n / retime = %d\n / rutime = %d\n / stime = %d\n\n", pid_child, type, retime, rutime, stime);

        ready[index] = ready[index] + retime;
        sleeping[index] = sleeping[index] + stime;
        turnaround[index] = turnaround[index] + retime + rutime + stime;
    }

    int media_ready[3];
    int media_sleeping[3];
    int media_turnaround[3];

    printf(stdout, " << Estatisticas Medias >> \n");
    printf(stdout, "\n");

    for (int x = 0; x<3; x++){
        printf(stdout, "> Tipo = %s / Processos: %d\n", get_type(x), p[x]);
        
        int number_of_p = p[x];

        media_ready[x] = ready[x]/number_of_p;
        media_sleeping[x] = sleeping[x]/number_of_p;
        media_turnaround[x] = turnaround[x]/number_of_p;
        
        printf(stdout, "> > Tempo READY(RUNNABLE) medio: %d\n", media_ready[x]);
        printf(stdout, "> > Tempo SLEEPING medio: %d\n", media_sleeping[x]);
        printf(stdout, "> > Tempo TURNAROUND medio: %d\n", media_turnaround[x]);
        printf(stdout, "\n");
    }

    exit();
}