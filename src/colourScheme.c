#include "colourScheme.h"


//Colour schemes, colsc = last square using the respective colour from colsv, ...
//colsv = foreground-colour,background-colour, ...
//In binary, format is RRRR RGGG GGGB BBBB.
#define C(r,g,b) (((r)&0xF8)<<8 | ((g)&0xFC)<<3 | ((b)&0xF8)>>3)

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

//Green
unsigned char colsc4[] =
{
    15,  // Walls
    19,  // Snipe generator
    31,  // Player
    126, // Text
    127, // Snipe generator
    129, // Snipes
    130, // Player bullets 1
    131, // Player bullets 2
    143, // Snipe bullets
    147, // Cloud 1/4
    151, // Cloud 2/4
    155, // Cloud 3/4
    159, // Cloud 4/4
    163, // Collision
    255, // Undefined
};
unsigned short colsv4[] =
{
    C(172, 230, 24), C(41, 85, 24),   // Walls
    C(0, 255, 205), C(222, 145, 8),   // Snipe generator
    C(0, 230, 230), C(41, 85, 24),    // Player
    C(255, 255, 255), C(82, 125, 74), // Text
    C(171, 200, 166), C(0, 0, 0),     // Snipe generator
    C(131, 255, 131), C(41, 85, 24),  // Snipes
    C(151, 255, 131), C(41, 85, 24),  // Player bullets 1
    C(255, 151, 231), C(41, 85, 24),  // Player bullets 2
    C(255, 186, 157), C(41, 85, 24),  // Snipe bullets
    C(64, 255, 0), C(41, 85, 24),     // Cloud 1/4
    C(144, 192, 0), C(41, 85, 24),    // Cloud 2/4
    C(144, 128, 0), C(41, 85, 24),    // Cloud 3/4
    C(64, 64, 0), C(41, 85, 24),      // Cloud 4/4
    C(255, 0, 0), C(41, 85, 24),      // Collision
    C(0, 0, 0), C(0, 0, 0),           // Undefined
};


colscheme colss[NUMCOLSS]={
  {colsc0, colsv0, " Blue       "},
  {colsc1, colsv1, " Monochrome "},
  {colsc2, colsv2, " Mild       "},
  {colsc3, colsv3, " Rainbow    "},
  {colsc4, colsv4, " Green tones"},
};
