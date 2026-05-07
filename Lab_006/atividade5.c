#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 6
#define VAGAS 3

int vagas_livres = VAGAS;
pthread_mutex_t mutex;
pthread_cond_t cond;

void* carro(void *arg) {
    int id = *(int*)arg;
    
    printf("Carro %d: tentando entrar...\n", id);

    pthread_mutex_lock(&mutex);
    while (vagas_livres == 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    vagas_livres--;
    printf("Carro %d: entrou na vaga. Vagas livres: %d\n", id, vagas_livres);
    pthread_mutex_unlock(&mutex);

    sleep(2); // simula tempo de uso

    pthread_mutex_lock(&mutex);
    vagas_livres++;
    printf("Carro %d: saiu da vaga. Vagas livres: %d\n", id, vagas_livres);
    pthread_cond_signal(&cond); // acorda um carro que esteja esperando
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, carro, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
