#include "colourScheme.h"


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


colscheme colss[NUMCOLSS]={
  {colsc0, colsv0, " Blue       "},
  {colsc1, colsv1, " Monochrome "},
  {colsc2, colsv2, " Mild       "},
  {colsc3, colsv3, " Rainbow    "}
};
