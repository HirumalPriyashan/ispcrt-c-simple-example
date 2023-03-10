#include <ispcrt/ispcrt.h>
#include <stdio.h>
#include <stdlib.h>

static ISPCRTError rt_error = ISPCRT_NO_ERROR;
static char *err_message = NULL;
static void ispcrt_error(ISPCRTError err_code, const char *message)
{
    rt_error = err_code;
    err_message = (char *)message;
}

static int run(const ISPCRTDeviceType device_type, const unsigned int N)
{
    ispcrtSetErrorFunc(ispcrt_error);
    ISPCRTDevice device = ispcrtGetDevice(device_type, 0);
    ISPCRTTaskQueue queue = ispcrtNewTaskQueue(device);

    // int *a = (int *)calloc(2 * N, sizeof(int)), *b = a + N;
    int a[20], b[20];
    for (int i = 0; i < N; i++)
        a[i] = N - i, b[i] = i;

    ISPCRTNewMemoryViewFlags flags;
    flags.allocType = ISPCRT_ALLOC_TYPE_DEVICE;
    ISPCRTMemoryView a_dev = ispcrtNewMemoryView(device, a, sizeof(int) * N, &flags);
    ISPCRTMemoryView b_dev = ispcrtNewMemoryView(device, b, sizeof(int) * N, &flags);
    ispcrtCopyToDevice(queue, a_dev);
    ispcrtCopyToDevice(queue, b_dev);

    size_t size = 2 * sizeof(int *) + 4 * sizeof(int);
    void **p = (void **)calloc(1, size);
    p[0] = ispcrtDevicePtr(a_dev);
    p[1] = ispcrtDevicePtr(b_dev);
    p[2] = calloc(1, (sizeof(int)));
    *((int *)(p[2])) = N;
    p[3] = calloc(1, (sizeof(int)));
    *((int *)(p[3])) = N;
    p[4] = calloc(1, (sizeof(int)));
    *((int *)(p[4])) = 1;
    p[5] = calloc(1, (sizeof(int)));
    *((int *)(p[5])) = 1;

    ISPCRTMemoryView p_dev = ispcrtNewMemoryView(device, p, size, &flags);

    ISPCRTModuleOptions options = {};
    ISPCRTModule module = ispcrtLoadModule(device, "vecadd", options);
    if (rt_error != ISPCRT_NO_ERROR)
        printf("%s ", err_message);
    ISPCRTKernel kernel = ispcrtNewKernel(device, module, "main_ispc");

    ispcrtCopyToDevice(queue, p_dev);
    void *res = ispcrtLaunch3D(queue, kernel, p_dev, 1, 1, 1);
    ispcrtCopyToHost(queue, a_dev);
    ispcrtSync(queue);
    free(p);

    for (unsigned i = 0; i < N; i++)
        printf("%d ", a[i]);

    for (int i = 0; i < N; i++)
        a[i] = i + 1, b[i] = 0;

    a_dev = ispcrtNewMemoryView(device, a, sizeof(int) * N, &flags);
    b_dev = ispcrtNewMemoryView(device, b, sizeof(int) * N, &flags);
    ispcrtCopyToDevice(queue, a_dev);
    ispcrtCopyToDevice(queue, b_dev);

    p = (void **)calloc(1, size);
    p[0] = ispcrtDevicePtr(a_dev);
    p[1] = ispcrtDevicePtr(b_dev);
    p[2] = calloc(1, (sizeof(int)));
    *((int *)(p[2])) = N;
    p[3] = calloc(1, (sizeof(int)));
    *((int *)(p[3])) = N;
    p[4] = calloc(1, (sizeof(int)));
    *((int *)(p[4])) = 1;
    p[5] = calloc(1, (sizeof(int)));
    *((int *)(p[5])) = 1;

    p_dev = ispcrtNewMemoryView(device, p, size, &flags);

    module = ispcrtLoadModule(device, "simple", options);
    kernel = ispcrtNewKernel(device, module, "main_ispc");

    ispcrtCopyToDevice(queue, p_dev);
    ispcrtLaunch3D(queue, kernel, p_dev, 1, 1, 1);
    ispcrtCopyToHost(queue, b_dev);
    ispcrtSync(queue);
    free(p);
    printf("\n");
    for (unsigned i = 0; i < N; i++)
        printf("%d ", b[i]);
    return 0;
}

int main(int argc, char *argv[])
{
    unsigned int SIZE = 20;

    int success = run(ISPCRT_DEVICE_TYPE_CPU, SIZE);
    return success;
}
