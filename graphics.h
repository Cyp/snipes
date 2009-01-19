#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "colourScheme.h"


typedef struct {
  int x, y, tx, ty, ox, oy;
  char *n;
  unsigned char *bitm;
} videomode;


int initGraphics(colscheme const *colours);  // Error = -1
void setColourScheme(colscheme const *colours);
void drawtiles();
void drawdisp(unsigned char *disp);
void setmodevars(int mode);
int switchmode(int mode);
int getmode();


extern char keyb[256];

extern videomode modes[8];

#endif //GRAPHICS_H_
