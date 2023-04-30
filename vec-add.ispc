task void foo_inner(uniform int32 *uniform a, uniform int32 const *uniform b,
                    uniform int32 const rows, uniform int32 const cols) {
  for (int i = 0; i < (cols + programCount - 1) / programCount; ++i)
    if (programIndex + i * programCount < cols)
      a[((uniform int32)taskIndex0) * cols + (varying int32)programIndex +
        i * programCount] = a[((uniform int32)taskIndex0) * cols +
                              (varying int32)programIndex + i * programCount] +
                            b[((uniform int32)taskIndex0) * cols +
                              (varying int32)programIndex + i * programCount];
}

export void foo(uniform int32 *uniform a, uniform int32 const *uniform b,
                uniform int32 const rows, uniform int32 const cols) {
  // assert(programCount == (cols));
  launch[rows] foo_inner(a, b, rows, cols);
}

export void nomp_ispc_foo(void *uniform _p) {
  void **uniform p = (void **uniform)_p;
  foo((int32 * uniform)(p[0]), (int32 * uniform)(p[1]), *((int32 *)(p[2])),
      *((int32 *)(p[3])));
}
