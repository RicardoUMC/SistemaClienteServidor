#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/types.h>

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

int obtiene_semaforo(key_t llave)
{
   int semid=semget(llave,1,IPC_CREAT|PERMISOS);
   if(semid==-1)
   {
      return -1;
   }
   semctl(semid,0,GETVAL,NULL);
   printf("\nValor del semáforo: %d\n", semid);
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

int ComprarBoletos(int disponibles)
{
   int i, cantidad;
   /*int semaforo_inicia;
   key_t llave_inicia = ftok("inicia", 't');
   semaforo_inicia = crea_semaforo(llave_inicia, 0);
   down(semaforo_inicia);

   int semaforo_termina;
   key_t llave_termina = ftok("termina", 't');
   semaforo_termina = crea_semaforo(llave_termina, 0);
   down(semaforo_termina);
   semaforo_wait = semget(llave_wait,1,IPC_CREAT|PERMISOS);
   if(semaforo_wait == -1)
   {
      printf("El servidor no pudo completar la transacción");
      pthread_exit(NULL);
   }
   down(semaforo_wait); */
   printf("[%i BOLETOS DISPONIBLES]\n", disponibles);
   printf("Ingrese la cantidad de boletos a comprar: ");
   scanf("%i", &cantidad);

   if(cantidad > disponibles)
   {
      printf("Lo sentimos, no tenemos tantos boletos disponibles :(\n");
      return 0;
   }

   printf("\nPid de compra: %d\n",getpid());
   for(i=1;i<=cantidad;i++)
   {
      printf("\tBoleto %d.\n",i);
      sleep(1);	
   }
   printf("Compra finalizada\n");

   return cantidad;
}

int main(void)
{
   int boletos_cantidad, i, *asientos;
   sem_t semaforo_mutex, semaforo_encendido, semaforo_espacio, memoria1;
   key_t llave2, llave3, llave_encendido;

   llave_encendido = ftok("Encendido", 'n');
   semaforo_encendido = obtiene_semaforo(llave_encendido);
   up(semaforo_encendido);

   /***************************************************/
   /*     Semáforo Mutex para el paso al servidor     */
   /***************************************************/
   key_t llave_mutex = ftok("Mutex", 'k'); 
   semaforo_mutex = obtiene_semaforo(llave_mutex);
   down(semaforo_mutex);     

   llave2 = ftok("Prueba2", 'l'); 
   semaforo_espacio = crea_semaforo(llave2,N);
   
   llave3 = ftok("Prueba3", 'o');
   memoria1 = shmget(llave3, sizeof(int), IPC_CREAT|0600);
   asientos = shmat(memoria1, 0, 0);
   
   int llave_stop = ftok("PruebaStop", 'p'); 
   int semaforo_stop = crea_semaforo(llave_stop, 1);
   down(semaforo_stop);         

   boletos_cantidad = ComprarBoletos(*asientos);
   *asientos = *asientos - boletos_cantidad;
   for (i = 0; i < boletos_cantidad; i++)
   {
      down(semaforo_espacio);
   }
   up(semaforo_stop);
   up(semaforo_mutex);
}
