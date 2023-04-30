#include "vec-add.h"
#include <ispcrt/ispcrt.h>
#include <stdio.h>
#include <stdlib.h>

static ISPCRTError rt_error = ISPCRT_NO_ERROR;
static char *err_message = NULL;
static void ispcrt_error(ISPCRTError err_code, const char *message) {
  rt_error = err_code;
  err_message = (char *)message;
}

static int run(const ISPCRTDeviceType device_type, int rows, int cols) {
  ispcrtSetErrorFunc(ispcrt_error);

  int a[256], b[256];
  const int n = rows * cols;
  for (unsigned i = 0; i < n; i++)
    a[i] = 2 * n - i, b[i] = i;
  ISPCRTDevice device = ispcrtGetDevice(device_type, 0);
  ISPCRTTaskQueue queue = ispcrtNewTaskQueue(device);

  ISPCRTNewMemoryViewFlags flags;
  flags.allocType = ISPCRT_ALLOC_TYPE_DEVICE;
  ISPCRTMemoryView a_dev =
      ispcrtNewMemoryView(device, a, sizeof(int) * n, &flags);
  ISPCRTMemoryView b_dev =
      ispcrtNewMemoryView(device, b, sizeof(int) * n, &flags);
  ispcrtCopyToDevice(queue, a_dev);
  ispcrtCopyToDevice(queue, b_dev);

  size_t size = 4 * sizeof(void *);
  void **p = (void **)calloc(1, size);
  p[0] = ispcrtDevicePtr(a_dev);
  p[1] = ispcrtDevicePtr(b_dev);
  p[2] = &rows;
  p[3] = &cols;
  nomp_ispc_foo(p);
  //   foo(a, b, rows, cols);
  for (unsigned i = 0; i < n; i++)
    printf("%d %s", a[i], (i % cols == cols - 1) ? "\n" : "");
  free(p);
  return 0;
}

int main(int argc, char *argv[]) {
  int success = run(ISPCRT_DEVICE_TYPE_CPU, 32, 4);
  success = run(ISPCRT_DEVICE_TYPE_CPU, 10, 16);
  return success;
}
