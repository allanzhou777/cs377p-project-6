#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <atomic>

#define _USE_MATH_DEFINES


double actual_pi = M_PI;
int num_threads = 16;
int steps = 100000;
int iterations = 100;
// std::atomic<double> pi_approx{0.0};
// double pi_approx = 0.0;
double step_size;
pthread_mutex_t pi_mutex;
double *sum_arr;



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
    sum_arr[thread_id] += 2 * step_size * f(x);
    x += step_size * num_threads;
  }

  // pthread_mutex_lock (&pi_mutex);
  // pi_approx += local_sum * 2;
  // auto current = pi_approx.load();
  // while (!pi_approx.compare_exchange_weak(current, current + local_sum * 2));
  // pthread_mutex_unlock (&pi_mutex);
  // sum_arr[thread_id] = local_sum * 2;


  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  uint64_t execTime;
  struct timespec tick, tock;
  step_size = 1.0 / steps;

  pthread_t threads[num_threads];            // array of thread identifiers
  struct ThreadData threadData[num_threads]; // array to hold thread data
  sum_arr = (double *)malloc(num_threads * sizeof(double));
  double pi_approx;

  clock_gettime(CLOCK_MONOTONIC_RAW, &tick);

  for (int iter = 0; iter < iterations; iter++)
  {
    int rc;

    for (int i = 0; i < num_threads; i++) {
      sum_arr[i] = 0.0;
    }

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

    for (int i = 0; i < num_threads; i++) {
      pi_approx += sum_arr[i];
    }
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
  execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;

  printf("-----%d threads-----\n", num_threads);
  printf("Avg elapsed time = %llu nanoseconds\n", (long long unsigned int)execTime / iterations);
  printf("Final pi approximation (%d steps): %.20f\n", steps, pi_approx / iterations);

  return EXIT_SUCCESS;
}