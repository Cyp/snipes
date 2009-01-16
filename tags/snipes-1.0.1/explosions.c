//Output: Random graphics

#include <stdio.h>

#define X 25
#define Y X

unsigned char bitm[X*Y*2];
#define get(x,y) (!(bitm[((x)>>3)+((y)<<1)*X]&  128>>((x)&7)))
#define stb(x,y)    bitm[((x)>>3)+((y)<<1)*X]|= 128>>((x)&7)
#define stw(x,y)    bitm[((x)>>3)+((y)<<1)*X]&=~128>>((x)&7)
#define st(x,y,s) (s)?(stw(x,y)):(stb(x,y))

int main() {
  FILE *f;
  int x, y, z, r;
  double i, j;
  unsigned short r2;
  f=fopen("/dev/random", "rb");
  for(z=0;z<4;++z) for(y=0;y<Y;++y) for(x=0;x<X;++x) {
    i=(x*2-X+1)/(X+1.); j=(y*2-Y+1)/(Y+1.);
    if(x>=y) {
      fread(&r2, 2, 1, f); r=r2;
      st(x+z*X*4,y,r<65536/(1+8*i*i+8*j*j)*(1-.25*z));
    } else st(x+z*X*4,y,get(y+z*X*4,x));
  }
  for(z=0;z<16;++z) if(z&3) for(y=0;y<Y;++y) for(x=0;x<X;++x)
    st(x+z*X,y,get(y+z*X-X,Y-1-x));
  fclose(f);
  f=fopen("tmp.pbm", "wb");
  fprintf(f, "P4\n%d %d\n", X*16, Y);
  fwrite(bitm, X*Y*2, 1, f);
  fclose(f);
  return(0);
}
