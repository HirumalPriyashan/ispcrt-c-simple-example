#include <ispcrt/ispcrt.h>
#include <stdio.h>
#include <stdlib.h>

static ISPCRTError rt_error = ISPCRT_NO_ERROR;
static char *err_message = NULL;
static void ispcrt_error(ISPCRTError err_code, const char *message) {
  rt_error = err_code;
  err_message = (char *)message;
}

static int run(const ISPCRTDeviceType device_type, unsigned int N) {
    ispcrtSetErrorFunc(ispcrt_error);

    unsigned *a = (unsigned *)calloc(2 * N, sizeof(unsigned)), *b = a + N;
    for (int i = 0; i < N; i++)
        a[i] = i, b[i] = N - i;
    ISPCRTDevice device = ispcrtGetDevice(device_type, 0);
    ISPCRTTaskQueue queue = ispcrtNewTaskQueue(device);

    ISPCRTNewMemoryViewFlags flags;
    flags.allocType = ISPCRT_ALLOC_TYPE_DEVICE;
    ISPCRTMemoryView a_dev = ispcrtNewMemoryView(device, a, sizeof(unsigned) * N, &flags);
    ISPCRTMemoryView b_dev = ispcrtNewMemoryView(device, b, sizeof(unsigned) * N, &flags);
    ispcrtCopyToDevice(queue, a_dev);
    ispcrtCopyToDevice(queue, b_dev);

    size_t size = 6 * sizeof(void *);
    int dim0 = N, dim1 = 1, dim2 = 1;
    void **p = (void **)calloc(1, size);
    p[0] = ispcrtDevicePtr(a_dev);
    p[1] = ispcrtDevicePtr(b_dev);
    p[2] = &N;
    p[3] = &dim0;
    p[4] = &dim1;
    p[5] = &dim2;
    for (int i = 0; i < 6; i++) {
    if (i < 2) {
        printf("p[%d] = %p\n", i, p[i]);
    } else {
        printf("p[%d] = %d\n", i, *(int *)p[i]);
    }
}
    ISPCRTMemoryView p_dev = ispcrtNewMemoryView(device, p, size, &flags);

    ISPCRTModuleOptions options = {};
    ISPCRTModule module = ispcrtLoadModule(device, "simple", options);
    if (rt_error != ISPCRT_NO_ERROR)
        printf("%s ", err_message);
    ISPCRTKernel kernel = ispcrtNewKernel(device, module, "main_ispc");

    ispcrtCopyToDevice(queue, p_dev);
    void *res = ispcrtLaunch3D(queue, kernel, p_dev, 1, 1, 1);
    ispcrtCopyToHost(queue, a_dev);
    ispcrtSync(queue);

    for (unsigned i = 0; i < N; i++)
        printf("%u ", a[i]);
    free(p);
    return 0;
}

int main(int argc, char *argv[]) {
    unsigned int SIZE = 20;

    int success = run(ISPCRT_DEVICE_TYPE_CPU, SIZE);
    return success;
}
