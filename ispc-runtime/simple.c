#include <ispcrt/ispcrt.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <math.h>

static ISPCRTError rt_error = ISPCRT_NO_ERROR;
static char *err_message = NULL;
static void ispcrt_error(ISPCRTError err_code, const char *message) {
  rt_error = err_code;
  err_message = (char *)message;
}

static int run(const ISPCRTDeviceType device_type, unsigned int N, int trials) {
  ispcrtSetErrorFunc(ispcrt_error);

  float *a = (float *)calloc(3 * N, sizeof(float)), *b = a + N, *c = b + N;
  for (int i = 0; i < N; i++)
    a[i] = i, b[i] = N - i;
  ISPCRTDevice device = ispcrtGetDevice(device_type, 0);
  ISPCRTTaskQueue queue = ispcrtNewTaskQueue(device);

  ISPCRTNewMemoryViewFlags flags;
  flags.allocType = ISPCRT_ALLOC_TYPE_DEVICE;
  ISPCRTMemoryView a_dev =
      ispcrtNewMemoryView(device, a, sizeof(float) * N, &flags);
  ISPCRTMemoryView b_dev =
      ispcrtNewMemoryView(device, b, sizeof(float) * N, &flags);
  ISPCRTMemoryView c_dev =
      ispcrtNewMemoryView(device, c, sizeof(float) * N, &flags);
  ispcrtCopyToDevice(queue, a_dev);
  ispcrtCopyToDevice(queue, b_dev);
  ispcrtCopyToDevice(queue, c_dev);

  size_t size = 7 * sizeof(void *);
  int dim0 = (N+7)/8, dim1 = 1, dim2 = 1;
  void **p = (void **)calloc(1, size);
  p[0] = ispcrtDevicePtr(c_dev);
  p[1] = ispcrtDevicePtr(a_dev);
  p[2] = ispcrtDevicePtr(b_dev);
  p[3] = &N;
  p[4] = &dim0;
  p[5] = &dim1;
  p[6] = &dim2;

  ISPCRTMemoryView p_dev = ispcrtNewMemoryView(device, p, size, &flags);

  ISPCRTModuleOptions options = {};
  ISPCRTModule module = ispcrtLoadModule(device, "simple", options);
  if (rt_error != ISPCRT_NO_ERROR)
    printf("%s ", err_message);
  ISPCRTKernel kernel = ispcrtNewKernel(device, module, "main_ispc");

  ispcrtCopyToDevice(queue, p_dev);
  clock_t time = clock();
  for (unsigned t = 0; t < trials; t++) {
    void *res = ispcrtLaunch3D(queue, kernel, p_dev, 1, 1, 1);
    ispcrtSync(queue);
  }
  time = clock() - time;

  ispcrtCopyToHost(queue, a_dev);

  printf("N: %u time: %e\n", N, (double)time / (CLOCKS_PER_SEC * trials));
  for (unsigned i = 0; i < N; i++)
    assert(fabs(c[i] - N) < 1e-8);

  free(p), free(a);
  return 0;
}

int main(int argc, char *argv[]) {
  unsigned N = (argc > 1 ? atoi(argv[1]) : 1000000);
  unsigned trials = 1000;

  int success = run(ISPCRT_DEVICE_TYPE_CPU, N, trials);
  return success;
}
