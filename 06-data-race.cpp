#include <omp.h>
#include <stdio.h>
#include <assert.h>
#include "utils.h"

int main(int argc, char** argv) {

  int a = 0, b = 0;

  #pragma omp parallel
  {
    #pragma omp sections
    {
      #pragma omp section
      {
        printf("thread %d executing section 1\n", omp_get_thread_num());
        a = 1;
        //#pragma omp flush(a)
        b = 2;
      }

      #pragma omp section
      {
        printf("thread %d executing section 2\n", omp_get_thread_num());
        if (b == 2) assert(a == 1); // may fail (writes to a and b not ordered without flush)
      }
    }
  }

  return 0;
}


// Synopsis
// The order of memory operations may not be the same as that of the program
// order. On a single thread the reordering respects sequential consistency i.e.
// the re-ordering will not change the final result. But when using multiple
// threads, the sequence of memory operation observed from another thread may be
// different than the program order.
//
// Weak sequential consistency
// - Memory operations are not ordered with respect to each other; i.e. memory
// operations may be re-ordered as long as it does not change the observed order
// on the same thread.
// - Memory operations are ordered with respect to synchronization operations
// such as barrier and flush
//
