struct Parameters {
    float *vin;
    float *vout;
    int    count;
};


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
    Parameters *uniform p = (Parameters * uniform) _p;
    simple(p->vin, p->vout, p->count);
}

#include "ispcrt.isph"
DEFINE_CPU_ENTRY_POINT(simple_ispc)
