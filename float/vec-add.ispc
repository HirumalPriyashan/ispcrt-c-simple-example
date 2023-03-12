task void foo_inner(uniform float *uniform a, uniform float const *uniform b, uniform int32 const N)
{
  a[((uniform int32) taskIndex0)] = a[((uniform int32) taskIndex0)] + b[((uniform int32) taskIndex0)];
}

task void main_ispc(void *uniform _p)
{
  void **uniform p = (void **uniform)_p;
  uniform int dim0 = *((int *)(p[3]));
  uniform int dim1 = *((int *)(p[4]));
  uniform int dim2 = *((int *)(p[5]));
  launch[dim0, dim1, dim2] foo_inner( (float *uniform)(p[0]),  (float *uniform)(p[1]),  *((int32 *)(p[2])));
}

#include "ispcrt.isph"
DEFINE_CPU_ENTRY_POINT(main_ispc)