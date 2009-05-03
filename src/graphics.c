#include "graphics.h"


#include <stdlib.h>
#include <SDL.h>

extern unsigned char _binary_img_bitm8x8_pbm_end;
extern unsigned char _binary_img_bitm10x10_pbm_end;
extern unsigned char _binary_img_bitm12x12_pbm_end;
extern unsigned char _binary_img_bitm14x14_pbm_end;
extern unsigned char _binary_img_bitm16x16_pbm_end;
extern unsigned char _binary_img_bitm20x20_pbm_end;
extern unsigned char _binary_img_bitm24x24_pbm_end;
extern unsigned char _binary_img_bitm25x25_pbm_end;

videomode modes[8]={
  { 640, 480, 8, 8, 0, 0, " 640x480  ", &_binary_img_bitm8x8_pbm_end   - 32*8*8},
  { 800, 600,10,10, 0, 0, " 800x600  ", &_binary_img_bitm10x10_pbm_end - 32*10*10},
  {1024, 768,12,12,32,24, " 1024x768 ", &_binary_img_bitm12x12_pbm_end - 32*12*12},
  {1152, 864,14,14,16,12, " 1152x864 ", &_binary_img_bitm14x14_pbm_end - 32*14*14},
  {1280, 960,16,16, 0, 0, " 1280x960 ", &_binary_img_bitm16x16_pbm_end - 32*16*16},
  {1600,1200,20,20, 0, 0, " 1600x1200", &_binary_img_bitm20x20_pbm_end - 32*20*20},
  {1920,1440,24,24, 0, 0, " 1920x1440", &_binary_img_bitm24x24_pbm_end - 32*24*24},
  {2048,1536,25,25,24,18, " 2048x1536", &_binary_img_bitm25x25_pbm_end - 32*25*25}
};


int bpp=2;
int tsx=8, tox=0, resx=640;
int tsy=8, toy=0, resy=480;
int mouse=SDL_ENABLE;
unsigned char *bitm = &_binary_img_bitm8x8_pbm_end - 32*8*8;
int curmode=0;

unsigned char odisp[2][80*60];
int bdisp;

char keyb[256];

int fullscreen=0;
SDL_Surface *scr=0;
SDL_Surface *tiles=0;

unsigned char *tilebit=0;

unsigned char *colsc;// = colss[0].c;
unsigned short *colsv;// = colss[0].v;


int initGraphics(colscheme const *colours)
{
  setColourScheme(colours);

  scr=SDL_SetVideoMode(resx, resy, 16, SDL_HWSURFACE|SDL_ANYFORMAT|SDL_DOUBLEBUF|fullscreen);
  if(!scr)
  {
    setmodevars(0);
    if(!(scr=SDL_SetVideoMode(resx, resy, 16, SDL_HWSURFACE|SDL_ANYFORMAT|SDL_DOUBLEBUF|fullscreen)))
    {
      printf("SDL_SetVideoMode failed: \"%s\"", SDL_GetError());
      return -1;
    }
  }
  SDL_ShowCursor(mouse);
  return 0;
}

void setColourScheme(colscheme const *colours)
{
    colsc = colours->c;
    colsv = colours->v;
}

void drawtiles() {
  int x, y, a, b, n;
  unsigned short fg, bg;
  if(tilebit) free(tilebit);
  if(!(tilebit=(unsigned char *)malloc(16*16*tsx*tsy*bpp))) return;
  fg=colsv[0]; bg=colsv[1]; n=0;
  for(b=0;b<16;++b) for(a=0;a<16;++a) {
    if((a|(b<<4))>colsc[n]) {
      fg=colsv[(++n)<<1]; bg=colsv[(n<<1)|1];
    }
    for(y=b*tsy;y<(b+1)*tsy;++y) for(x=a*tsx;x<(a+1)*tsx;++x)
      ((unsigned short *)tilebit)[x+y*16*tsx]=(bitm[(x>>3)+y*2*tsx]&(128>>(x&7)))?bg:fg;
  }
  if(tiles) SDL_FreeSurface(tiles);
  tiles=SDL_CreateRGBSurfaceFrom(tilebit, 16*tsx, 16*tsy, 2*8, 16*tsx*bpp, 31<<11, 63<<5, 31, 0);

  // Redraw everything on next redraw.
  memset(odisp, 255, sizeof(odisp));
}

void drawdisp(unsigned char *disp) {
  SDL_Rect f, t;
  int x, y;
  unsigned char *c=disp;
  for(y=0;y<60;++y) for(x=0;x<80;++x,++c) if(bdisp<0||odisp[bdisp][x+y*80]!=*c) {
    f.w=t.w=tsx; f.h=t.h=tsy;
    f.x=((*c)&15)*tsx;
    f.y=((*c)>>4)*tsy;
    t.x=x*tsx+tox;
    t.y=y*tsy+toy;
    if(tiles&&0<=SDL_BlitSurface(tiles, &f, scr, &t))
      odisp[bdisp&1][x+y*80]=*c;
    else
      odisp[bdisp&1][x+y*80]=255;
  }

  SDL_Flip(scr);
}

void setmodevars(int mode) {
  curmode=mode; fullscreen=mode/100?SDL_FULLSCREEN:0; mouse=mode/100?SDL_DISABLE:SDL_ENABLE; mode=mode%100;
  tsx=modes[mode].tx; tsy=modes[mode].ty; tox=modes[mode].ox; toy=modes[mode].oy;
  resx=modes[mode].x; resy=modes[mode].y;
  bitm=modes[mode].bitm;
}

int switchmode(int nmode) {
  SDL_Surface *newscr;
  int old=curmode;
  setmodevars(nmode);
  if(!(newscr=SDL_SetVideoMode(resx, resy, 16, SDL_HWSURFACE|SDL_ANYFORMAT|SDL_DOUBLEBUF|fullscreen))) {
    setmodevars(old);
    return(0);
  } else SDL_ShowCursor(mouse);
  scr=newscr;
  memset(keyb, 0, sizeof(keyb));
  memset(odisp, 255, sizeof(odisp));
  drawtiles();
  return(0);
}

int getmode()
{
    return curmode;
}
