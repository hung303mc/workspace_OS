#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREAD 10
#define NUM_EXE 10

int sharedVar = 0;  /* shared variable */
int numReader = 0;  /* Number of reader thread */

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;    /* mutex lock for shared var */
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;    /* mutex lock for number of reader*/
pthread_cond_t c_write = PTHREAD_COND_INITIALIZER; /* writer waits on this cond var */

void *writer (void *param);
void *reader (void *param);

int main(int argc, char *argv[]) {

  pthread_t tid1, tid2;  /* thread identifiers */
  int i;

  /* create the threads; may be any number, in general */
  if(pthread_create(&tid1, NULL, writer, NULL) != 0) {
    fprintf(stderr, "Unable to create writer thread\n");
    exit(1);
  }
  if(pthread_create(&tid2, NULL, reader, NULL) != 0) {
    fprintf(stderr, "Unable to create reader thread\n");
    exit(1);
  }
  /* wait for created thread to exit */
  for (i = 0; i < NUM_THREAD; ++i)
  {
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);    
  }
  printf("Parent quiting\n");
  return 0;
}

/* Write value(s) */
void *writer(void *param) {
  int cntWri;
  for (cntWri = 0; cntWri < NUM_EXE; ++cntWri)
  {
    /* wait rands*/
    usleep((rand()%50)*1000*5);

    /* Write into shared Var */
    pthread_mutex_lock (&m1);  

    /*Block if there is any reader thread*/
    while(numReader != 0)
    {
      printf("numReader > 0, wait to write ...\n");
      pthread_cond_wait(&c_write,&m1);
    }
    sharedVar = cntWri;
    /* if executing here, there is no read thread */
    pthread_mutex_unlock (&m1);

    printf("write value %d \n",cntWri);
    printf("number of reader is %d \n",numReader); /*It should be = 0*/
  }
  return 0;
}

/* Read value(s); Note the reader never terminates */
void *reader(void *param) {
  int cntRead;
  int readVal;

    pthread_mutex_lock(&m2);
    /*increase numReader*/    
      numReader++;
    pthread_mutex_unlock(&m2);
  for (int cntRead = 0; cntRead < NUM_EXE; ++cntRead)
  {
    /* wait rands*/
    usleep((rand()%50)*1000*20);

  
    /* Read from shared Var update numReader*/
    pthread_mutex_lock (&m1);
    /* if executing here, buffer not empty so remove element */
      readVal = sharedVar;        
    pthread_mutex_unlock (&m1);
  
    printf("read value %d \n",readVal);
    printf("number of reader is %d \n",numReader);  
  }
    pthread_mutex_lock(&m2);
    /*increase numReader*/    
      numReader--;
    pthread_mutex_unlock(&m2);

    /* If no reader --> generate signal to writer*/
    if(numReader == 0){
      pthread_cond_broadcast(&c_write);
    }

  return 0;
}