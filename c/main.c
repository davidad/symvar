#include "symvar.c"

void main() {
  struct symvar *v, *w;
  v=sv_init_d(2.0);
  w=sv_init_d(1.0);
  sv_print(v);
  sv_print(w);
  v = sv_ss(ADD,v,w);
  v = sv_sd(EXP,v,3.0);
  sv_print(v);
  return;
}
