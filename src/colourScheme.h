#ifndef COLOURSCHEME_H_
#define COLOURSCHEME_H_

typedef struct {
  unsigned char *c;
  unsigned short *v;
  char *n;
} colscheme;


#define NUMCOLSS 5
extern colscheme colss[NUMCOLSS];

#endif //COLOURSCHEME_H_
