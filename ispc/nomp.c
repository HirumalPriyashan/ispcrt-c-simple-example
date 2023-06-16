#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void vec_add(float *c, float *a, float *b, unsigned N) {
#pragma nomp for transform("transforms", "foo")
  for (unsigned i = 0; i < N; i++)
    c[i] = a[i] + b[i];
}
int main(int argc, char *argv[]) {
  unsigned N = (argc > 1 ? atoi(argv[1]) : 1000000);
  unsigned trials = 1000;

  float *a = (float *)calloc(3 * N, sizeof(float));
  float *b = a + N, *c = b + N;
  for (int i = 0; i < N; i++)
    a[i] = i, b[i] = N - i;

#pragma nomp init(argc, argv)

#pragma nomp update(to : a[0, N], b[0, N])
#pragma nomp update(alloc : c[0, N])

  // Do a warm up run
  for (unsigned t = 0; t < 0; t++)
    vec_add(c, a, b, N);

#pragma nomp sync
  clock_t t = clock();
  for (unsigned t = 0; t < trials; t++)
    vec_add(c, a, b, N);
  t = clock() - t;

#pragma nomp update(from : c[0, N])
#pragma nomp update(free : a[0, N], b[0, N], c[0, N])

  printf("N: %u time: %e\n", N, (double)t / (CLOCKS_PER_SEC * trials));

  for (unsigned i = 0; i < N; i++)
    assert(fabs(c[i] - N) < 1e-8);
    // printf("%f ", c[i]);

#pragma nomp finalize
  // free(a);
  return 0;
}
