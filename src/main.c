#include "random.h"
#include "colourScheme.h"
#include "skillLevel.h"
#include "settings.h"
#include "snipes.h"
#include "graphics.h"
#include "snipeSound.h"


#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL.h>


int handler(SDL_Event *);

unsigned int timer;


int main(int aa, char **bb)
{
  FILE *f;
  int ll, z;
  char chr[5], ch;
  int newmode = 0;
  char const *snipeSettingFile = "/.snipesettings";
  char const *homePath = getenv("HOME");
  if(homePath == NULL)
      homePath = ".";
  char *snipeSettingPath = malloc(strlen(homePath) + strlen(snipeSettingFile) + 1);
  if(snipeSettingPath == NULL)
  {
      printf("Out of memory!\n");
      return 1;
  }
  strcpy(snipeSettingPath, homePath);
  strcat(snipeSettingPath, snipeSettingFile);
  if((f=fopen("/dev/urandom", "rb"))!=0) {
    fread(&ll, sizeof(int), 1, f); fclose(f);
    seedr((unsigned int)ll);
  } else {
    seedr((unsigned int)time(0));
  }
  if((f=fopen(snipeSettingPath, "rb"))!=0) {
    if(fread(&chr, 1, 5, f)) {
      if(chr[0]>='A'&&chr[0]<='Z') gameskill=&skills[lastskill=chr[0]-'A'];
      if(chr[1]>='0'&&chr[1]<='9') gamelevel=&levels[lastlevel=chr[1]-'0'];
      if(chr[2]>='a'&&chr[2]<='h') newmode=chr[2]-'a';
      if(chr[2]>='A'&&chr[2]<='H') newmode=chr[2]-'A'+100;
      if(chr[3]=='S') enableSound(1); else if(chr[3]=='Q') enableSound(0); //Sound/Quiet//
      if(chr[4]>='0'&&chr[4]<='0'+NUMCOLSS-1) { curcols=chr[4]-'0'; }
    }
    fclose(f);
  }
  for(z=1;z<aa;++z) for(ll=0; (ch = bb[z][ll]) != 0; ++ll) {
    if((ch|32)>='a'&&(ch|32)<='z') gameskill=&skills[lastskill=(ch|32)-'a'];
    if(ch>='0'&&ch<='9') gamelevel=&levels[lastlevel=ch-'0'];
    if(ch==';' || (ch==':' && (ch = bb[z][ll+1]))) {
      ++ll;
      if(ch=='y'||ch=='Y') enableSound(1); else if(ch=='n'||ch=='N') enableSound(0);
      else if(ch>='a'&&ch<='h') newmode=ch-'a';
      else if(ch>='A'&&ch<='H') newmode=ch-'A'+100;
      else if(ch>='0'&&ch<='0'+NUMCOLSS-1) { curcols=ch-'0'; }
    }
  }
  setmodevars(newmode);
  if(0>SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER)) {
    printf("SDL_Init failed: \"%s\"", SDL_GetError()); return(1); }
  atexit(SDL_Quit); //Not sure if this should do anything...
  SDL_WM_SetCaption("Snipes 2D", "Snipes");
  if(initGraphics(&colss[curcols]) < 0)
    return 1;

  SDL_Event m;
  int noquit=1;
  m.type=/*Init*/24; handler(&m);
  while(noquit&&!saidtoquit) {
    while(noquit&&SDL_PollEvent(&m)) {
      noquit=handler(&m);
    }
    if(!noquit) break;
    m.type=/*Timer*/25;
    noquit=handler(&m);
    SDL_Delay(1);
  }
  cleanupSnipes();
  if((f=fopen(snipeSettingPath, "wb"))!=0) {
    chr[0]=lastskill+'A';
    chr[1]=lastlevel+'0';
    if(getmode()<100)
      chr[2]=getmode()+'a';
    else
      chr[2]=getmode()+'A'-100;
    chr[3] = isSoundEnabled()? 'S' : 'Q';
    chr[4]=curcols+'0';
    fwrite(&chr, 1, 5, f);
    fclose(f);
  }
#ifdef RECOVERBITMAPS
  SDL_Surface s; int x; char *nm[8]={"bitm8x8.bmp", "bitm10x10.bmp", "bitm12x12.bmp", "bitm14x14.bmp", "bitm16x16.bmp", "bitm20x20.bmp", "bitm24x24.bmp", "bitm25x25.bmp"};
  for(x=0;x<8;++x) {
    setmodevars(x);
    drawtiles();
    SDL_SaveBMP(tiles, nm[x]);
  }
#endif
  SDL_Quit();
  free(snipeSettingPath);
  return 0;
}

int const working=1;

int nextt, frac91;
int handler(SDL_Event *ev) {
  int curd, curt, x;
  switch(ev->type) {
    case /*Init*/24: memset(keyb, 0, sizeof(keyb)); drawtiles(); break;
    case SDL_QUIT: return(0);
    case /*Timer*/25: if(!working) break; curd=(nextt-(curt=SDL_GetTicks()))*91+frac91; if(curd>0) break; 
                   if(curd<-5000) { nextt=curt; frac91=0; } else { nextt+=54; frac91+=86; if(frac91>=91) { ++nextt; frac91-=91; }}
      for(curt=16;curt>=0;--curt) lcurd[curt+1]=lcurd[curt]; lcurd[0]=curd;
      //The remainder of this case statement executes 18.2 times a second (in theory).
      if(newmode>=0)
      {
          if(switchmode(newmode)) { printf("This didn't happen.\n"); } else newmode=-1;
      }
      //TODO: Should probably check for lost surfaces...
      tickgame(); drawdisp(disp); playsounds();
      //Keypresses have been detected, clear bits that say they were just pressed.
      for(x=0;x<256;++x) keyb[x]&=~0x0C; break;
    case SDL_KEYDOWN: x=ev->key.keysym.scancode-8; if(!(keyb[x&255]&(1<<(x>>8)))) keyb[x&255]|= (5<<(x>>8)); break;
    case SDL_KEYUP:   x=ev->key.keysym.scancode-8;                                keyb[x&255]&=~(1<<(x>>8)); break;
  }
  return(1);
}
