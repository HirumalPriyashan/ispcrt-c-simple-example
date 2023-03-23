task void foo_inner(uniform float *uniform a, uniform int32 const N)
{
  uniform int32 t;

  t = 0;
  for (uniform int32 j = 0; j <= 9; ++j)
  {
    t = t + 1;
    if (j == 5)
      break;
  }
  a[((uniform int32) taskIndex0)] = t;
}

task void nomp_foo_inner_wrapper(void *uniform _p)
{
  void **uniform p = (void **uniform)_p;
  uniform int dim0 = *((int *)(p[2]));
  uniform int dim1 = *((int *)(p[3]));
  uniform int dim2 = *((int *)(p[4]));
  launch[dim0, dim1, dim2] foo_inner( (float *uniform)(p[0]),  *((int32 *)(p[1])));
}

#include "ispcrt.isph"
DEFINE_CPU_ENTRY_POINT(nomp_foo_inner_wrapper)