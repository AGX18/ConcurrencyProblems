#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

volatile int n = 1;
pthread_mutex_t lock;
pthread_cond_t cond_var;
int turn = 0;

void *fizz(void *arg) {
  while (1) {
    pthread_mutex_lock(&lock);
    while (turn != 2) {
      pthread_cond_wait(&cond_var, &lock);
    }
    if (n % 3 == 0) {
      printf("\tfizz");
    }
    turn--;
    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&cond_var);
  }
}

void *buzz(void *arg) {
  while (1) {
    pthread_mutex_lock(&lock);
    while (turn != 1) {
      pthread_cond_wait(&cond_var, &lock);
    }
    if (n % 5 == 0) {
      printf("\tbuzz");
    }
    turn--;
    pthread_cond_broadcast(&cond_var);

    pthread_mutex_unlock(&lock);
  }
}

int main () {
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&cond_var, NULL);
  // start two threads, running fizz and buzz respectively
  pthread_t t1, t2;
  pthread_create(&t1, NULL, fizz, NULL);
  pthread_create(&t2, NULL, buzz, NULL);
  // every 100ms, update n randomly from the range [0, 16), indefinitely

  while (1) {
    pthread_mutex_lock(&lock);
    while (turn != 0) {
      pthread_cond_wait(&cond_var, &lock);
    }
    n = rand() & 0xf;
    printf("\n%d:", n);
    turn = 2;
    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&cond_var);
    usleep(100000);
  }
}
