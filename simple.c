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

    ISPCRTNewMemoryViewFlags flags;
    flags.allocType = ISPCRT_ALLOC_TYPE_DEVICE;
    ISPCRTMemoryView a_dev = ispcrtNewMemoryView(device, a, sizeof(float) * N, &flags);
    ISPCRTMemoryView b_dev = ispcrtNewMemoryView(device, b, sizeof(float) * N, &flags);
    ISPCRTMemoryView c_dev = ispcrtNewMemoryView(device, c, sizeof(float) * N, &flags);

    size_t size = 3 * sizeof(float *) + 4 * sizeof(int);
    void **p = (void **)calloc(1, size);
    p[0] = (float*)ispcrtDevicePtr(a_dev);
    p[1] = (float*)ispcrtDevicePtr(b_dev);
    p[2] = (float*)ispcrtDevicePtr(c_dev);
    p[3] = calloc(1, (sizeof(int)));
    *((int *)(p[3])) = N;
    p[4] = calloc(1, (sizeof(int)));
    *((int *)(p[4])) = N;
    p[5] = calloc(1, (sizeof(int)));
    *((int *)(p[5])) = 1;
    p[6] = calloc(1, (sizeof(int)));
    *((int *)(p[6])) = 1;

    ISPCRTMemoryView p_dev = ispcrtNewMemoryView(device, p, size, &flags);

    ISPCRTModuleOptions options = {};
    ISPCRTModule module = ispcrtLoadModule(device, "simple", options);
    ISPCRTKernel kernel = ispcrtNewKernel(device, module, "vec_add_inner");

    ISPCRTTaskQueue queue = ispcrtNewTaskQueue(device);
    ispcrtCopyToDevice(queue, p_dev);
    ispcrtCopyToDevice(queue, a_dev);
    ispcrtCopyToDevice(queue, b_dev);
    // ispcrtCopyToDevice(queue, c_dev);
    void *res = ispcrtLaunch3D(queue, kernel, p_dev, 1, 1, 1);
    ispcrtCopyToHost(queue, c_dev);
    ispcrtSync(queue);

    for (unsigned i = 0; i < N; i++)
        printf("%f", c[i]);
    free(p);
    return 0;
}

int main(int argc, char *argv[]) {
    unsigned int SIZE = 20;

    int success = run(ISPCRT_DEVICE_TYPE_CPU, SIZE);
    return success;
}
