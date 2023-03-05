task void simple(uniform float vin[], uniform float vout[],
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
    launch simple((float *)(p[0]), (float *)(p[1]), *((int *)(p[2])));
}

export void simple_ispc_cpu_entry_point(void *uniform parameters, uniform int dim0, uniform int dim1,
                                           uniform int dim2) {
    launch[dim0, dim1, dim2] simple_ispc(parameters);
}

// ispc simple.ispc -o simple.dev.o
// /usr/bin/cc -fPIC -shared -o libxe_simple.so simple.dev.o
// /usr/bin/cc simple.c -fPIE -o host_simple -lispcrt -Wl,-rpath,../ispc-v1.18.1-linux/lib64
