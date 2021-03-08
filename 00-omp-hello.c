// OpenMP hello world
// $ gcc -fopenmp 00-omp-hello.c && ./a.out
// # export OMP_NUM_THREADS = <number-of-threads>

#include <omp.h>
#include <stdio.h>

int main(int argc, char** argv) {
  printf("maximum number of threads = %d\n", omp_get_max_threads());

  #pragma omp parallel
  {
    printf("hello world from thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
  }

  return 0;
}
