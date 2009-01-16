typedef struct {
  int x, y, tx, ty, ox, oy;
  char *n;
  unsigned char *bitm;
} videomode;

unsigned char colsc[]={           19,       31,          126,     129,      135,      143,         147,         151,        155,        159,         161,         255};
//Colour scheme, foreground-colour,background-colour, ...
//In binary, format is RRRR RGGG GGGB BBBB.
unsigned short colsv[]={65535,16, 63519,16, 65535,63488, 2016,16, 65504,16, 63488,16, 64512,33808, 64512,16904, 64512,8456, 64512,4240, 65504,33808, 63488,33808};

/*
//Black and white version
unsigned char colsc[]={255};
unsigned short colsv[]={65535,0};
*/

#include "bitms.h"

videomode modes[8]={
  { 640, 480, 8, 8, 0, 0, " 640x480  ", bitm8x8},
  { 800, 600,10,10, 0, 0, " 800x600  ", bitm10x10},
  {1024, 768,12,12,32,24, " 1024x768 ", bitm12x12},
  {1152, 864,14,14,16,12, " 1152x864 ", bitm14x14},
  {1280, 960,16,16, 0, 0, " 1280x960 ", bitm16x16},
  {1600,1200,20,20, 0, 0, " 1600x1200", bitm20x20},
  {1920,1440,24,24, 0, 0, " 1920x1440", bitm24x24},
  {2048,1536,25,25,24,18, " 2048x1536", bitm25x25}
};
