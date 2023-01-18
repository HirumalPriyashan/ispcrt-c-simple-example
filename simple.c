#include "ispcrt.h"
#include <stdio.h>
#include <stdlib.h>

struct Parameters {
    float *vin;
    float *vout;
    int count;
};

static int run(const ISPCRTDeviceType device_type, const unsigned int SIZE) {
    float *vin = (float *)calloc(2 * SIZE, sizeof(float)), *vout = vin + SIZE;
    for (int i = 0; i < SIZE; i++)
        vin[i] = i;

    ISPCRTDevice device = ispcrtGetDevice(device_type, 0);

    ISPCRTNewMemoryViewFlags flags;
    flags.allocType = ISPCRT_ALLOC_TYPE_DEVICE;
    ISPCRTMemoryView vin_dev = ispcrtNewMemoryView(device, vin, sizeof(float) * SIZE, &flags);
    ISPCRTMemoryView vout_dev = ispcrtNewMemoryView(device, vout, sizeof(float) * SIZE, &flags);

    struct Parameters *p = (struct Parameters *)calloc(1, sizeof(struct Parameters));

    p->vin = (float*)ispcrtDevicePtr(vin_dev);
    p->vout = (float*)ispcrtDevicePtr(vout_dev);
    p->count = SIZE;

    ISPCRTMemoryView p_dev = ispcrtNewMemoryView(device, p, sizeof(struct Parameters), &flags);

    ISPCRTModuleOptions options = {};
    ISPCRTModule module = ispcrtLoadModule(device, "xe_simple", options);
    ISPCRTKernel kernel = ispcrtNewKernel(device, module, "simple_ispc");

    ISPCRTTaskQueue queue = ispcrtNewTaskQueue(device);
    ispcrtCopyToDevice(queue, p_dev);
    ispcrtCopyToDevice(queue, vin_dev);
    void *res = ispcrtLaunch1D(queue, kernel, p_dev, 1);
    ispcrtCopyToHost(queue, vout_dev);
    ispcrtSync(queue);

    for (int i = 0; i < SIZE; i++) {
        printf("%d: simple(%f): %f\n", i, vin[i], vout[i]);
    }
    free(vin), free(p);
    return 0;
}

int main(int argc, char *argv[]) {
    unsigned int SIZE = 16;

    int success = run(ISPCRT_DEVICE_TYPE_CPU, SIZE);
    return success;
}
