task void add(uniform float a[], uniform float b[], uniform float c[]) {
    c[taskIndex0] = a[taskIndex0] + b[taskIndex0];
}

task void vec_add_inner(void *uniform _p)
{
    void **uniform p = (void **uniform)_p;
    uniform int dim0 = *((int *)(p[4]));
    uniform int dim1 = *((int *)(p[5]));
    uniform int dim2 = *((int *)(p[6]));
    launch[dim0, dim1, dim2] add((float *uniform)(p[0]), (float *uniform)(p[1]), (float *uniform)(p[2]));
}

export void vec_add_inner_cpu_entry_point(
        void *uniform parameters, uniform int dim0, uniform int dim1,
        uniform int dim2) {
    launch[dim0, dim1, dim2] vec_add_inner(parameters);
}
