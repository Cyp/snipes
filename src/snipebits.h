typedef struct {
  int x, y, tx, ty, ox, oy;
  char *n;
  unsigned char *bitm;
} videomode;

typedef struct {
  unsigned char *c;
  unsigned short *v;
  char *n;
} colscheme;

//Colour schemes, colsc = last square using the respective colour from colsv, ...
//colsv = foreground-colour,background-colour, ...
//In binary, format is RRRR RGGG GGGB BBBB.

unsigned char colsc0[]={       19,       31,         126,     129,      135,      143,         147,         151,        155,        159,         161,         255};
unsigned short colsv0[]={65535,16, 63519,16, 65535,63488, 2016,16, 65504,16, 63488,16, 64512,33808, 64512,16904, 64512,8456, 64512,4240, 65504,33808, 63488,33808};

//Black and white (and grey) version
unsigned char colsc1[]={      31,     126,     255};
unsigned short colsv1[]={65535,0, 65535,33808, 65535,0};

//Mild version of default
unsigned char colsc2[]={      19,      31,         126,     129,     135,     143,         147,         151,        155,        159,         161,         255};
unsigned short colsv2[]={65535,8, 64543,8, 65535,49672, 34800,8, 65520,8, 64528,8, 64512,33808, 64512,16904, 64512,8456, 64512,4240, 65504,33808, 63488,33808};

//Rainbow
unsigned char colsc3[]={           2,           3,           4,           7,           8,          15,         19,          31,      123,      124,      126,        127,      128,       129,         130,         131,         143,     147,     151,     155,     159,       161,         255};
unsigned short colsv3[]={63488,16904, 64512,16904, 63488,16904, 64512,16904, 63488,16904, 64512,16904, 2016,16904, 34784,16904, 65535,31, 50719,31, 65535,31, 2047,16904, 31,16904, 543,16904, 65504,16904, 65055,16904, 49183,16904, 64543,0, 64533,0, 64522,0, 64512,0, 512,65535, 18400,65535};

unsigned char *colsc=colsc0;
unsigned short *colsv=colsv0;

#define NUMCOLSS 4
colscheme colss[NUMCOLSS]={
  {colsc0, colsv0, " Blue       "},
  {colsc1, colsv1, " Monochrome "},
  {colsc2, colsv2, " Mild       "},
  {colsc3, colsv3, " Rainbow    "}
};

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
