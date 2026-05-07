%%writefile primo_pc.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

int N;
int M;

// Buffer circular e controle
int *Buffer;
int in = 0;
int out = 0;

// Semáforos e variável para resultado
sem_t slotVazio;
sem_t slotCheio;
sem_t mutexGeral;
int total_primos = 0;

// Função de primalidade (Fornecida)
int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

void *produtor(void *arg) {
    int i;
    for (i = 1; i <= N; i++) {
        sem_wait(&slotVazio);
        sem_wait(&mutexGeral);
        
        Buffer[in] = i; // Insere os números de 1 até N
        in = (in + 1) % M;
        
        sem_post(&mutexGeral);
        sem_post(&slotCheio);
    }

    // Insere o valor sentinela (-1) para avisar o consumidor que acabou
    sem_wait(&slotVazio);
    sem_wait(&mutexGeral);
    Buffer[in] = -1;
    in = (in + 1) % M;
    sem_post(&mutexGeral);
    sem_post(&slotCheio);

    pthread_exit(NULL);
}

// Thread Consumidora
void *consumidor(void *arg) {
    int item;
    
    while (1) {
        sem_wait(&slotCheio);
        sem_wait(&mutexGeral);
        
        item = Buffer[out];
        out = (out + 1) % M;
        
        sem_post(&mutexGeral);
        sem_post(&slotVazio);

        // Verifica a sentinela para encerrar o loop
        if (item == -1) {
            break; 
        }

        // Se não for sentinela, verifica se é primo
        if (ehPrimo(item)) {
            total_primos++;
        }
    }
    
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    pthread_t tid_prod, tid_cons;

    if (scanf("%d %d", &N, &M) != 2) {
        printf("Erro na leitura da entrada.\n");
        return 1;
    }

    Buffer = (int *)malloc(M * sizeof(int));
    if (Buffer == NULL) {
        printf("Erro de alocação de memória.\n");
        return 1;
    }

    sem_init(&mutexGeral, 0, 1);
    sem_init(&slotVazio, 0, M);  // Inicia com M slots livres
    sem_init(&slotCheio, 0, 0);  // Inicia com 0 slots ocupados

    // Criação das threads
    pthread_create(&tid_prod, NULL, produtor, NULL);
    pthread_create(&tid_cons, NULL, consumidor, NULL);

    // Aguarda o término das threads
    pthread_join(tid_prod, NULL);
    pthread_join(tid_cons, NULL);

    // Imprime o resultado final
    printf("Total de primos encontrados: %d\n", total_primos);

    free(Buffer);
    sem_destroy(&mutexGeral);
    sem_destroy(&slotVazio);
    sem_destroy(&slotCheio);

    return 0;
}
