#include "random.h"


unsigned int rseeda[624];
int rseedu=624;

void seedr(unsigned int s) {
  int n;
  rseeda[0]=s;
  for(n=1;n<624;++n) rseeda[n]=69069u*rseeda[n-1];
}

void gennewr() {
  int n;
  unsigned int v;
  for(n=0;n<227;++n) {
    v=(rseeda[n]&0x80000000)|(rseeda[n+1]&0x7fffffff);
    rseeda[n]=rseeda[n+397]^(v>>1)^(0x9908b0df&(0-(v&1)));
  }
  for(;n<623;++n) {
    v=(rseeda[n]&0x80000000)|(rseeda[n+1]&0x7fffffff);
    rseeda[n]=rseeda[n-227]^(v>>1)^(0x9908b0df&(0-(v&1)));
  }
  v=(rseeda[623]&0x80000000)|(rseeda[0]&0x7fffffff);
  rseeda[623]=rseeda[396]^(v>>1)^(0x9908b0df&(0-(v&1)));
  rseedu=0;
}

unsigned int rnd() {
  if(rseedu>=624) gennewr();
  int v=rseeda[rseedu++];
  v^=v>>11;
  v^=(v<<7)&0x9d2c5680;
  v^=(v<<15)&0xefc60000;
  v^=v>>18;
  return(v);
}

unsigned int rndr(unsigned int m) {
  unsigned int v, c=(0u-m)/m;
  while((v=rnd())/m>c);
  return(v%m);
}
