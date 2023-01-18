static void simple(uniform float vin[], uniform float vout[],
                   uniform int count) {
    foreach (index = 0 ... count) {
        float v = vin[index];
        if (v < 5.)
            v = v * v;
        else
            v = v*2;
        vout[index] = v-1;
    }
}

task void simple_ispc(void *uniform _p) {
    void **uniform p = (void **uniform)_p;
    simple((float *)(p[0]), (float *)(p[1]), *((int *)(p[2])));
}

#include "ispcrt.isph"
DEFINE_CPU_ENTRY_POINT(simple_ispc)
