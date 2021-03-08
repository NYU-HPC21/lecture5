/*
 * OpenMP Example based on code in Tim Mattson's Tutorial
 *
 * [slides](http://openmp.org/mp-documents/Intro_To_OpenMP_Mattson.pdf)
 * [video](http://goo.gl/EaxWjY)
 *
 * This computes pi various ways
 */

#include <stdio.h>
#include <omp.h>

#define NUM_STEPS 4000000000
#define MAX_NUM_THREADS 60

double compute_pi1()
{
  long i;
  double x, pi;
  double sum = 0.0;
  const double step = 1.0/NUM_STEPS;

  for(i = 0; i < NUM_STEPS; ++i)
  {
    x = (i+0.5)*step;
    sum += 4.0/(1.0+x*x);
  }
  pi = step * sum;

  return pi;
}


/*
 * Example of false sharing
 */
double compute_pi2()
{
  int j, num_threads;
  double pi;
  double sum[MAX_NUM_THREADS];
  const double step = 1.0/NUM_STEPS;

#pragma omp parallel
  {
    long i;
    double x;
    int nt = omp_get_num_threads();
    int id = omp_get_thread_num();

    if(id == 0)
      num_threads = nt;

    sum[id] = 0.0;
    for(i = id; i < NUM_STEPS; i+=nt)
    {
      x = (i+0.5)*step;
      sum[id] += 4.0/(1.0+x*x);
    }
  }

  pi = 0.0;
  for(j=0; j<num_threads; ++j)
    pi += sum[j]*step;

  return pi;
}

/*
 * Data race bug
 */
double compute_pi3()
{
  double pi = 0.0;
  const double step = 1.0/NUM_STEPS;

#pragma omp parallel
  {
    long i;
    double sum = 0.0;
    double x;
    int nt = omp_get_num_threads();
    int id = omp_get_thread_num();

    for(i = id; i < NUM_STEPS; i+=nt)
    {
      x = (i+0.5)*step;
      sum += 4.0/(1.0+x*x);
    }
    pi += sum*step;
  }

  return pi;
}

double compute_pi4()
{
  double pi = 0.0;
  const double step = 1.0/NUM_STEPS;

#pragma omp parallel
  {
    long i;
    double sum = 0.0;
    double x;
    int nt = omp_get_num_threads();
    int id = omp_get_thread_num();

    for(i = id; i < NUM_STEPS; i+=nt)
    {
      x = (i+0.5)*step;
      sum += 4.0/(1.0+x*x);
    }
#pragma omp critical
    pi += sum*step;
  }

  return pi;
}

double compute_pi5()
{
  double pi = 0.;
  const double step = 1.0/NUM_STEPS;

#pragma omp parallel
  {
    long i;
    double sum = 0.0;
    double x;
    int nt = omp_get_num_threads();
    int id = omp_get_thread_num();

    for(i = id; i < NUM_STEPS; i+=nt)
    {
      x = (i+0.5)*step;
      sum += 4.0/(1.0+x*x);
    }
#pragma omp atomic
    pi += sum*step;
  }

  return pi;
}

double compute_pi6()
{
  long i;
  double pi;
  double sum = 0.0;
  const double step = 1.0/NUM_STEPS;

#pragma omp parallel for reduction(+:sum)
  for(i = 0; i < NUM_STEPS; ++i)
  {
    double x = (i+0.5)*step;
    sum += 4.0/(1.0+x*x);
  }
  pi = step * sum;

  return pi;
}

int main(int argc, char *argv[])
{
  double tic, toc, pi;

  tic = omp_get_wtime();
  pi = compute_pi1();
  toc = omp_get_wtime();
  printf("pi1: %25.16e in %g s\n", pi,  toc-tic);

  tic = omp_get_wtime();
  pi = compute_pi2();
  toc = omp_get_wtime();
  printf("pi2: %25.16e in %g s\n", pi,  toc-tic);

  tic = omp_get_wtime();
  pi = compute_pi3();
  toc = omp_get_wtime();
  printf("pi3: %25.16e in %g s\n", pi,  toc-tic);

  tic = omp_get_wtime();
  pi = compute_pi4();
  toc = omp_get_wtime();
  printf("pi4: %25.16e in %g s\n", pi,  toc-tic);

  tic = omp_get_wtime();
  pi = compute_pi5();
  toc = omp_get_wtime();
  printf("pi5: %25.16e in %g s\n", pi,  toc-tic);

  tic = omp_get_wtime();
  pi = compute_pi6();
  toc = omp_get_wtime();
  printf("pi6: %25.16e in %g s\n", pi,  toc-tic);

  printf("using %d threads \n",MAX_NUM_THREADS);

  return 0;
}

