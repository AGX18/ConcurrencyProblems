#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 4

#include "cputime.h"

typedef struct msort_args {
  int *arr;
  int n;
} msort_args;

void *start(void *arg) {
  msort_args *args = (msort_args *)arg;
  msort(args->arr, args->n);
  return NULL;
}

void merge(int *arr, int n, int mid) {
  int left = 0, right = mid, i;
  int *x = malloc(n * sizeof(int));
  // copy the ith item from either the left or right part
  for (i = 0; i < n; i++) {
    if (right == n)
      x[i] = arr[left++];
    else if (left == mid)
      x[i] = arr[right++];
    else if (arr[right] < arr[left])
      x[i] = arr[right++];
    else
      x[i] = arr[left++];
  }
  // transfer from temporary array back to given one
  for (i = 0; i < n; i++)
    arr[i] = x[i];
  free(x);
}

void msort(int *arr, int n) {
  if (n < 2)
    return;
  int mid = n / 2;
  msort(arr, mid);
  msort(arr + mid, n - mid);
  merge(arr, n, mid);
}

void thread_msort(int *arr, int n) {
  int num_threads = NUM_THREADS;
  pthread_t threads[num_threads];
  msort_args *args_arr[num_threads];
  
  for (int i = 0; i < num_threads; i++) {
      args_arr[i] = malloc(sizeof(msort_args));
      args_arr[i]->arr = arr + i * (n / num_threads);
      
      if (i == num_threads - 1)
          args_arr[i]->n = n - i * (n / num_threads);
      else
          args_arr[i]->n = n / num_threads;
          
      pthread_create(&threads[i], NULL, start, (void*)args_arr[i]);
  }
  
  // Wait for all threads to complete
  for (int i = 0; i < num_threads; i++) {
      pthread_join(threads[i], NULL);
      free(args_arr[i]);
  }

  for (int i = 1; i < num_threads; i++) {
    if (i == num_threads - 1)
      merge(arr, n, i * (n / num_threads));
    else
      merge(arr, (i + 1) * (n / num_threads), i * (n / num_threads));
  }

}

int main () {
  int n = 1 << 24;
  int *arr = malloc(n * sizeof(int));
  // populate array with n many random integers
  srand(1234);
  for (int i = 0; i < n; i++)
    arr[i] = rand();

  fprintf(stderr, "Sorting %d random integers\n", n);

  // actually sort, and time cpu use
  struct profile_times t;
  profile_start(&t);
  thread_msort(arr, n);
  profile_log(&t);



  // assert that the output is sorted
  for (int i = 0; i < n - 1; i++)
    if (arr[i] > arr[i + 1]) {
      printf("error: arr[%d] = %d > arr[%d] = %d", i, arr[i], i + 1,
             arr[i + 1]);
      return 1;
    }

  free(arr);
    return 0;
}
