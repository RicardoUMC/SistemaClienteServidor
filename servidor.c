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

/*
 * Se define un sem_t que es un entero para
 * el mejor manejo de variables del programa
 */
typedef int sem_t;

int crea_semaforo(key_t llave,int valor_inicial)
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
   // int semaforo_inicia;
   // key_t llave_inicia = ftok("inicia", 't');
   // semaforo_inicia = crea_semaforo(llave_inicia, 0);
   // down(semaforo_inicia);

   // int semaforo_termina;
   // key_t llave_termina = ftok("termina", 'u');
   // semaforo_termina = crea_semaforo(llave_termina, 0);
   // down(semaforo_termina);
   
   // int semaforo_wait;
   // key_t llave_wait = ftok("wait", 'v');
   // semaforo_wait = semget(llave_wait,1,IPC_CREAT|PERMISOS);
   // if(semaforo_wait == -1)
   // {
   //    printf("El servidor no pudo completar la transacción");
   //    pthread_exit(NULL);
   // }
   // up(semaforo_wait);
   pthread_exit(NULL);
}

int main(void) 
{
   int memoria1, *asientos;
   sem_t semaforo_encendido, semaforo_mutex;
   key_t llave3, llave_encendido;

   /***************************************************/
   /*            Inicialización del Mutex             */
   /***************************************************/
   /*
    * Es necesario que el servidor inicialice el mutex
    * para que los clientes solo accedan al valor que 
    * se tenga en ese momento.
    */
   key_t llave_mutex = ftok("Mutex", 'k'); 
   semaforo_mutex = crea_semaforo(llave_mutex, 1);

   /***************************************************/
   /*  Memoria compartida para cantidad de asientos   */
   /***************************************************/
   llave3 = ftok("Prueba3", 'o');
   memoria1 = shmget(llave3, sizeof(int), IPC_CREAT|0600);
   asientos = shmat(memoria1, 0, 0);
   *asientos = N;

   llave_encendido = ftok("Encendido", 'n');
   semaforo_encendido = crea_semaforo(llave_encendido, 0);
   int llave_stop = ftok("PruebaStop", 'p'); 
   int semaforo_stop = crea_semaforo(llave_stop, 0);
   
   while (1)
   {
      printf("\nServidor dormido (1)...\n");
      down(semaforo_encendido);

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
   
   /*
   while (1)
   {
      printf("Servidor dormido...\n");
      down(semaforo_encendido);
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
   */   
}