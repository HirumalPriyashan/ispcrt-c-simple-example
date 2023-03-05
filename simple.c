#include <ispcrt/ispcrt.h>
#include <stdio.h>
#include <stdlib.h>

static ISPCRTError rt_error = ISPCRT_NO_ERROR;
static char *err_message = NULL;
static void ispcrt_error(ISPCRTError err_code, const char *message) {
  rt_error = err_code;
  err_message = (char *)message;
}

static int run(const ISPCRTDeviceType device_type, const unsigned int N) {
    ispcrtSetErrorFunc(ispcrt_error);

    float *a = (float *)calloc(3 * N, sizeof(float)), *b = a + N, *c = b + N;
    for (int i = 0; i < N; i++)
        a[i] = i, b[i] = N - i;
    ISPCRTDevice device = ispcrtGetDevice(device_type, 0);
    ISPCRTTaskQueue queue = ispcrtNewTaskQueue(device);

    ISPCRTNewMemoryViewFlags flags;
    flags.allocType = ISPCRT_ALLOC_TYPE_DEVICE;
    ISPCRTMemoryView a_dev = ispcrtNewMemoryView(device, a, sizeof(float) * N, &flags);
    ISPCRTMemoryView b_dev = ispcrtNewMemoryView(device, b, sizeof(float) * N, &flags);
    ISPCRTMemoryView c_dev = ispcrtNewMemoryView(device, c, sizeof(float) * N, &flags);
    ispcrtCopyToDevice(queue, a_dev);
    ispcrtCopyToDevice(queue, b_dev);
    ispcrtCopyToDevice(queue, c_dev);

    // err = nomp_run(id, 4, "a", NOMP_PTR, sizeof(TEST_TYPE), a, "b", NOMP_PTR,
    //              sizeof(TEST_TYPE), b, "c", NOMP_PTR, sizeof(TEST_TYPE), c, "N",
    //              NOMP_INTEGER, sizeof(int), &n);
    size_t size = 3 * sizeof(float *) + 3 * sizeof(int);
    void **p = (void **)calloc(1, size);
    // p[0] = calloc(1, (sizeof(float *)));
    p[0] = ispcrtDevicePtr(a_dev);
    // p[1] = calloc(1, (sizeof(float *)));
    p[1] = ispcrtDevicePtr(b_dev);
    // p[2] = calloc(1, (sizeof(float *)));
    p[2] = ispcrtDevicePtr(c_dev);
    p[3] = calloc(1, (sizeof(int)));
    *((int *)(p[3])) = N;
    p[4] = calloc(1, (sizeof(int)));
    *((int *)(p[4])) = 1;
    p[5] = calloc(1, (sizeof(int)));
    *((int *)(p[5])) = 1;

    ISPCRTMemoryView p_dev = ispcrtNewMemoryView(device, p, size, &flags);

    ISPCRTModuleOptions options = {};
    ISPCRTModule module = ispcrtLoadModule(device, "simple", options);
    if (rt_error != ISPCRT_NO_ERROR)
        printf("%s ", err_message);
    ISPCRTKernel kernel = ispcrtNewKernel(device, module, "main_ispc");

    ispcrtCopyToDevice(queue, p_dev);
    void *res = ispcrtLaunch3D(queue, kernel, p_dev, 1, 1, 1);
    ispcrtCopyToHost(queue, c_dev);
    ispcrtSync(queue);

    for (unsigned i = 0; i < N; i++)
        printf("%f ", c[i]);
    free(p);
    return 0;
}

int main(int argc, char *argv[]) {
    unsigned int SIZE = 20;

    int success = run(ISPCRT_DEVICE_TYPE_CPU, SIZE);
    return success;
}
