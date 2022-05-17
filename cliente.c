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

int ComprarBoletos(int disponibles)
{
   int i, cantidad;
   int semaforo_inicia;
   key_t llave_inicia = ftok("inicia", 't');
   semaforo_inicia = Crea_semaforo(llave_inicia, 0);
   down(semaforo_inicia);

   int semaforo_termina;
   key_t llave_termina = ftok("termina", 't');
   semaforo_termina = Crea_semaforo(llave_termina, 0);
   down(semaforo_termina);
   /* semaforo_wait = semget(llave_wait,1,IPC_CREAT|PERMISOS);
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
   // cliente no funciona correctamente
   int boletos_cantidad, i, *asientos;
   int semaforo_mutex, semaforo_estado, semaforo_espacio, memoria1;
   key_t llave1, llave2, llave3, llave_estado;

   llave_estado = ftok("Estado", 'n');
   semaforo_estado = Crea_semaforo(llave_estado, 0);
   down(semaforo_estado);

   /***************************************************/
   /*     Semáforo Mutex para el paso al servidor     */
   /***************************************************/
   llave1 = ftok("Prueba1", 'k'); 
   // semaforo_mutex = Crea_semaforo(llave1, 1);
   int semid=semget(llave1,1,IPC_CREAT|PERMISOS);
   if(semid==-1)
   {
      return -1;
   }
   semaforo_mutex = semctl(semid,0,SETVAL,NULL);
   down(semaforo_mutex);     

   llave2 = ftok("Prueba2", 'l'); 
   semaforo_espacio = Crea_semaforo(llave2,N);
   
   llave3 = ftok("Prueba3", 'o');
   memoria1 = shmget(llave3, sizeof(int), IPC_CREAT|0600);
   asientos = shmat(memoria1, 0, 0);
   
   int llave_stop = ftok("PruebaStop", 'p'); 
   int semaforo_stop = Crea_semaforo(llave_stop, 1);
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
