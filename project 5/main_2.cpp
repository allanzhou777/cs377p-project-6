#include <pthread.h> /*used in other parts of the assignment */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h> /* for uint64  */
#include <time.h>   /* for clock_gettime */
#include <atomic>   /*used in other parts of the assignment */

#define _USE_MATH_DEFINES
double actual_pi = M_PI;
int numThreads = 4;


struct ThreadData {
  int thread_id;
};

double f(double x)
{
  return (1.0 / sqrt(1 - x * x));
}






double pi_approximation (int steps) {
  double x = 0;
  double pi_approx = 0.0;
  double step_size = 1.0 / steps;
  for (int i = 0; i < steps; i++) {
    pi_approx += step_size * f(x);
    x += step_size;
  }
  return pi_approx * 2;
}




int main(int argc, char *argv[])
{
  uint64_t execTime; /*time in nanoseconds */
  struct timespec tick, tock;
  clock_gettime(CLOCK_MONOTONIC_RAW, &tick);



  int l = 10; 
  int r = 10000000;
  double pi_l = actual_pi * 0.99;
  double pi_r = actual_pi * 1.01;

  while (l < r) {
    int m = l / 2 + r / 2;
    double pi_approx_M = pi_approximation (m);
    // printf("l: %d     r: %d      pi_approx for m = %d: %.5f\n", l, r, m, pi_approx_M);
    // printf ("pi_l: %d\n pi_r: %d\n", pi_l, pi_r);
    if (pi_l <= pi_approx_M && pi_approx_M <= pi_r) {
      r = m - 1;
    }
    else {
      l = m + 1;
    }
  }



  clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
  execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;






  printf("elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int)execTime);
  printf("pi_approximation (%d iterations): %.6f\n", (l + r)/2, pi_approximation ((l + r) / 2));
  // printf("sequentially derived pi: %.20f\n", pi);
  // printf("actual pi:               %.20f\n", actual_pi);
  return 0;
}