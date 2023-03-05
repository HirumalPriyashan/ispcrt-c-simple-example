task void add(uniform float a[], uniform float b[], uniform float c[]) {
    c[taskIndex0] = a[taskIndex0] + b[taskIndex0];
}

task void main_ispc(void *uniform _p)
{
    void **uniform p = (void **uniform)_p;
    uniform int dim0 = *((int *)(p[3]));
    uniform int dim1 = *((int *)(p[4]));
    uniform int dim2 = *((int *)(p[5]));
    launch[dim0, dim1, dim2] add((float *uniform)(p[0]), (float *uniform)(p[1]), (float *uniform)(p[2]));
}

#include "ispcrt.isph"
DEFINE_CPU_ENTRY_POINT(main_ispc)
