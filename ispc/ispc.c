#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include KERNEL_FILE

int main(int argc, char *argv[]) {
  unsigned N = (argc > 1 ? atoi(argv[1]) : 1000000);
  unsigned trials = (argc > 2 ? atoi(argv[2]) : 1000);

  float *a = (float *)calloc(3 * N, sizeof(float)), *b = a + N, *c = b + N;
  for (int i = 0; i < N; i++)
    a[i] = i, b[i] = N - i;

  for (unsigned t = 0; t < 100; t++)
    vec_add(a, b, c, N);

  clock_t time = clock();
  for (unsigned t = 0; t < trials; t++)
    vec_add(a, b, c, N);
  time = clock() - time;

  for (unsigned i = 0; i < N; i++)
    assert(fabs(c[i] - N) < 1e-8);

  printf("N: %u time: %e\n", N, (double)time / (CLOCKS_PER_SEC * trials));

  free(a);
  return 0;
}
