#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#define _USE_MATH_DEFINES
double actual_pi = M_PI;
int num_threads = 16;
int steps = 100000;
int iterations = 100;
double pi_approx = 0.0;
double step_size;

struct ThreadData
{
  int thread_id;
};

double f(double x)
{
  return (1.0 / sqrt(1 - x * x));
}

// approximate pi
void *pi_approximation(void *arg)
{
  struct ThreadData *data = (struct ThreadData *)arg;
  int thread_id = data->thread_id;
  double local_sum = 0.0;
  double x = thread_id * step_size;

  for (int i = thread_id; i < steps; i += num_threads)
  {
    local_sum += step_size * f(x);
    x += step_size * num_threads;
  }

  pi_approx += local_sum * 2;

  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  uint64_t execTime;
  struct timespec tick, tock;
  step_size = 1.0 / steps;

  pthread_t threads[num_threads];
  struct ThreadData threadData[num_threads];

  clock_gettime(CLOCK_MONOTONIC_RAW, &tick);

  for (int iter = 0; iter < iterations; iter++)
  {
    int rc;
    pi_approx = 0.0;

    // for a single iteration, run each computation in num_threads threads
    for (int i = 0; i < num_threads; i++)
    {
      threadData[i].thread_id = i;
      if ((rc = pthread_create(&threads[i], NULL, pi_approximation, &threadData[i])))
      {
        fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
        return EXIT_FAILURE;
      }
    }

    // wait for all threads to complete
    for (int i = 0; i < num_threads; i++)
    {
      pthread_join(threads[i], NULL);
    }
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
  execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;

  printf("-----%d threads-----\n", num_threads);
  printf("Avg elapsed time = %llu nanoseconds\n", (long long unsigned int)execTime / iterations);
  printf("Final pi approximation (%d steps): %.20f\n", steps, pi_approx);

  return EXIT_SUCCESS;
}
