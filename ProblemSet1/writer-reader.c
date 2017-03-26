#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int sharedVar = 0;  /* shared variable */
int add = 0;        /* place to add next element */
int rem = 0;        /* place to remove next element */
int num = 0;        /* number elements in buffer */

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;    /* mutex lock for buffer */
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER; /* reader waits on this cond var */
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER; /* writer waits on this cond var */

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
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  printf("Parent quiting\n");

  return 0;
}

/* Produce value(s) */
void *writer(void *param) {

  int i;
  for (i=1; i<=20; i++) {
    
    /* Insert into buffer */
    pthread_mutex_lock (&m);  
      if (num > BUF_SIZE) {
        exit(1);  /* overflow */
      }

      while (num == BUF_SIZE) {  /* block if buffer is full */
        pthread_cond_wait (&c_prod, &m);
      }
      
      /* if executing here, buffer not full so add element */
      buffer[add] = i;
      add = (add+1) % BUF_SIZE;
      num++;
    pthread_mutex_unlock (&m);

    pthread_cond_signal (&c_cons);
    printf ("writer: inserted %d\n", i);
    fflush (stdout);
  }

  printf("writer quiting\n");
  fflush(stdout);
  return 0;
}

/* Consume value(s); Note the reader never terminates */
void *reader(void *param) {

  int i;

  while(1) {

    pthread_mutex_lock (&m);
      if (num < 0) {
        exit(1);
      } /* underflow */

      while (num == 0) {  /* block if buffer empty */
        pthread_cond_wait (&c_cons, &m);
      }

      /* if executing here, buffer not empty so remove element */
      i = buffer[rem];
      rem = (rem+1) % BUF_SIZE;
      num--;
    pthread_mutex_unlock (&m);

    pthread_cond_signal (&c_prod);
    printf ("Consume value %d\n", i);  fflush(stdout);

  }
  return 0;
}