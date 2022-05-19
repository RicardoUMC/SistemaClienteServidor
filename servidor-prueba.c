#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/types.h>
#include<pthread.h>

#define PERMISOS 0644
#define N 5

int Crea_semaforo(key_t llave,int valor_inicial)
{
    int semid=semget(llave,1,IPC_CREAT|PERMISOS);
    if(semid==-1)
    {
        return -1;
    }
    semctl(semid,0,SETVAL,valor_inicial);
    return semid;
}

void down(int semid)
{
    struct sembuf op_p[]={0,-1,0};
    semop(semid,op_p,1);
}

void up(int semid)
{
    struct sembuf op_v[]={0,+1,0};
    semop(semid,op_v,1);
}

void *Hilo1(void *argumentos)
{
    pthread_exit(NULL);
}

int main(void)
{
    int memoria1, *asientos;
    int semaforo_estado;
    key_t llave3, llave_estado;

    /***************************************************/
   /*  Memoria compartida para cantidad de asientos   */
   /***************************************************/
    llave3 = ftok("Prueba3", 'o');
    memoria1 = shmget(llave3, sizeof(int), IPC_CREAT|0600);
    asientos = shmat(memoria1, 0, 0);
    *asientos = N;

    llave_estado = ftok("Estado", 'n');
    semaforo_estado = Crea_semaforo(llave_estado, 0);
    int llave_stop = ftok("PruebaStop", 'p');
    int semaforo_stop = Crea_semaforo(llave_stop, 0);
    
    while (1)
    {
        printf("Servidor dormido...\n");
        down(semaforo_estado);
        printf("Servidor despierto...\n");

        pthread_t id_hilo1;
        
        printf("\nCreacion del hilo...\n");
        pthread_create(&id_hilo1, NULL, Hilo1, NULL);
        printf("\nHilo creado. Esperando su finalizacion...\n");
        printf("\nEsperando cliente...\n");
        down(semaforo_stop);
        printf("\nTermina cliente...\n");
        pthread_join(id_hilo1, NULL);
        printf("\nHilo finalizado...\n-----------------------------------------------------------------\n");
    }
}