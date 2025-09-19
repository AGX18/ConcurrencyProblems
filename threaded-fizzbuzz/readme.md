# the Ineffecient Solution without a Condition Variable
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

volatile int n = 1;
pthread_mutex_t lock;
int b = 1, f = 1;
int turn = 0;

void *fizz(void *arg) {
  while (1) {
    pthread_mutex_lock(&lock);
    if (n % 3 == 0 && f == 0) {
      printf("\tfizz");
      f = 1;
      b = 0;
    }
    turn--;
    pthread_mutex_unlock(&lock);
  }
}

void *buzz(void *arg) {
  while (1) {
    pthread_mutex_lock(&lock);
    if (n % 5 == 0 && b == 0) {
      printf("\tbuzz");
      b = 1;
    }
    turn--;

    pthread_mutex_unlock(&lock);
  }
}

int main () {
  pthread_mutex_init(&lock, NULL);
  // start two threads, running fizz and buzz respectively
  pthread_t t1, t2;
  pthread_create(&t1, NULL, fizz, NULL);
  pthread_create(&t2, NULL, buzz, NULL);
  // every 100ms, update n randomly from the range [0, 16), indefinitely

  while (1) {
    pthread_mutex_lock(&lock);
    n = rand() & 0xf;
    printf("\n%d:", n);
    turn = 2;
    b = 1;
    f = 0;
    pthread_mutex_unlock(&lock);
    usleep(100000);
  }
}

```