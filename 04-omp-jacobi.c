/* Jacobi smoothing to solve -u''=f
 * Global vector has N inner unknowns.
 * gcc -fopenmp -lm 04-omp-jacobi.c && ./a.out 200000 100
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>

/* compuate global residual, assuming ghost values are updated */
double compute_residual(double *u, int N, double invhsq)
{
  int i;
  double tmp, res = 0.0;
#pragma omp parallel for reduction (+:res)
  for (i = 1; i <= N; i++){
    tmp = ((2.0*u[i] - u[i-1] - u[i+1]) * invhsq - 1);
    res += tmp * tmp;
  }
  return sqrt(res);
}


int main(int argc, char * argv[])
{
  int i, N, iter, max_iters;

  sscanf(argv[1], "%d", &N);
  sscanf(argv[2], "%d", &max_iters);

  /* timing */
  double t = omp_get_wtime();

  /* Allocation of vectors, including left and right ghost points */
  double* u    = (double *) calloc(sizeof(double), N+2);
  double* unew = (double *) calloc(sizeof(double), N+2);

  double h = 1.0 / (N + 1);
  double hsq = h*h;
  double invhsq = 1./hsq;
  double res, res0, tol = 1e-5;

  /* initial residual */
  res0 = compute_residual(u, N, invhsq);
  res = res0;
  u[0] = u[N+1] = 0.0;
  double omega = 1.0; //2./3;

  for (iter = 0; iter < max_iters && res/res0 > tol; iter++) {

    /* Jacobi step for all the inner points */
#pragma omp parallel for
    for (i = 1; i <= N; i++){
      unew[i] =  u[i] + omega * 0.5 * (hsq + u[i - 1] + u[i + 1] - 2*u[i]);
    }

    /* flip pointers; that's faster than memcpy  */
    // memcpy(u,unew,(N+2)*sizeof(double));
    double* utemp = u;
    u = unew;
    unew = utemp;
    if (0 == (iter % 1)) {
      res = compute_residual(u, N, invhsq);
      printf("Iter %d: Residual: %g\n", iter, res);
    }
  }

  /* Clean up */
  free(u);
  free(unew);

  /* timing */
  t = omp_get_wtime() - t;
  printf("Time elapsed is %f.\n", t);
  return 0;
}
