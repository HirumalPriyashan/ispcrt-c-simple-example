task void loopy_kernel_inner(uniform float *uniform c, uniform float const *uniform a, uniform float const *uniform b, uniform uint32 const N)
{
  if (-1 + -8 * ((uniform int32) taskIndex0) + -1 * (varying int32) programIndex + N >= 0)
    c[8 * ((uniform int32) taskIndex0) + (varying int32) programIndex] = a[8 * ((uniform int32) taskIndex0) + (varying int32) programIndex] + b[8 * ((uniform int32) taskIndex0) + (varying int32) programIndex];
}

task void main_ispc(void *uniform _p)
{
   void **uniform p = (void **uniform)_p;
  uniform int dim0 = *((int *)(p[4]));
  uniform int dim1 = *((int *)(p[5]));
  uniform int dim2 = *((int *)(p[6]));
  launch[dim0, dim1, dim2] loopy_kernel_inner( (float *uniform)(p[0]),  (float *uniform)(p[1]),  (float *uniform)(p[2]),  *((uint32 *)(p[3])));
}

#include "ispcrt.isph"
DEFINE_CPU_ENTRY_POINT(main_ispc)
