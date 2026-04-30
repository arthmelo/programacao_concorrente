#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int n_hellos = 0;  
pthread_mutex_t mutex;
pthread_cond_t cond;

#define NTHREADS_A 2
#define NTHREADS_B 2
#define TOTAL_THREADS (NTHREADS_A + NTHREADS_B)

void *A (void *t) {
  printf("HELLO\n");

  pthread_mutex_lock(&mutex);
  n_hellos++;

  if(n_hellos == NTHREADS_A) {
      pthread_cond_broadcast(&cond);
  }
  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}

void *B (void *t) {
  pthread_mutex_lock(&mutex);
  
  while (n_hellos < NTHREADS_A) {
     pthread_cond_wait(&cond, &mutex);
  }
  pthread_mutex_unlock(&mutex);

  printf("BYEBYE\n");

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t threads[TOTAL_THREADS];

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  for (int i = 0; i < NTHREADS_A; i++) {
    pthread_create(&threads[i], NULL, A, NULL);
  }

  for (int i = 0; i < NTHREADS_B; i++) {
    pthread_create(&threads[NTHREADS_A + i], NULL, B, NULL);
  }

  for (int i = 0; i < TOTAL_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
  return 0;
}
