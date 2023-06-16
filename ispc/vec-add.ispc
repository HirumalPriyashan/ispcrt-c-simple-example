export void vec_add(uniform float a[], uniform float b[], uniform float c[],
                    uniform int count) {
  foreach (index = 0 ... count) {
    c[index] = a[index] + b[index];
  }
}


