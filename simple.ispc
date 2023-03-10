task void simple(uniform int32 *uniform vin, uniform int32 *uniform vout,
                   uniform int count) {
    foreach (index = 0 ... count) {
        float v = vin[index];
        if (v < 5.)
            v = v * v;
        else
            v = v*2;
        vout[index] = v;
    }
}

task void main_ispc(void *uniform _p)
{
  void **uniform p = (void **uniform)_p;
  uniform int dim0 = *((int *)(p[3]));
  uniform int dim1 = *((int *)(p[4]));
  uniform int dim2 = *((int *)(p[5]));
  launch[dim0, dim1, dim2] simple( (int32 *uniform)(p[0]),  (int32 *uniform)(p[1]),  *((int32 *)(p[2])));
}

#include "ispcrt.isph"
DEFINE_CPU_ENTRY_POINT(main_ispc)

// ispc simple.ispc -o simple.dev.o
// /usr/bin/cc -fPIC -shared -o libxe_simple.so simple.dev.o
// /usr/bin/cc simple.c -fPIE -o host_simple -lispcrt -Wl,-rpath,../ispc-v1.18.1-linux/lib64
