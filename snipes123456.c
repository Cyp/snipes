#include <SDL.h>
//#define WIN32_BLOATED_AND_UNSTABLE
//#define INITGUID
//#exclude <windows.h>
//#include <mmsystem.h>
#include <malloc.h>
#include <stdio.h>
//#include <ddraw.h>
#include <time.h>
#include <math.h>
//#include <stdio.h>
///
///ALL
///

#include "snipebits.h"
//#include "SnipesBitmaps.h"

void seedr(unsigned int);
unsigned int rnd();
unsigned int rndr(unsigned int);
char *ddinit();
void dduninit();
void sndinit();
void snduninit();
void playsounds();
void drawtiles();
void drawdisp();
void setmodevars(int);
int switchmode(int);
void drawgame();
void tickgame();
int checkkey();
void initgame();
void initmaze();
void smobj(int);
int nobj(int);
void kobj(int);
void placeobj(int, int, int, unsigned char *);
void drawobj(int, int, int, unsigned char *);
int checkobj(int, int, int, int);
int trace(int *, int *, int, int, int *);
void tickobj();
int snitrace(int, int, int, int, int);
int sniscan(int, int, int, int);
int hapscan(int, int, int, int);
int keyb57();

int bpp=2;
int tsx=8, tox=0, resx=640;
int tsy=8, toy=0, resy=480;
unsigned char *bitm=bitm8x8;
int curmode=0;
int working=1;
int newmode=-1;

unsigned char disp[80*60];
unsigned char odisp[2][80*60];
int bdisp;

int psound=0;

char keyb[256];

//int mzx=4*12, mzy=35*12, mzcx=4, mzcy=35;
//int mzx=23*12, mzy=23*12, mzcx=23, mzcy=23;
int mzx/*=23*12*/, mzy/*=23*12*/, mzcx/*=23*/, mzcy/*=23*/;
int mzlx, mzly, mzlx2, mzly2, mzlsh;
//int rmw=28;
int rmw/*=53*/;
//int rmw/*=53*/;

unsigned char *maze=0;
int gametime=0, gametime2=0, gamescore=0, gamelives=0, gamestate=-1;
int nummotd=0, nummot=0, numsnid=0, numsni=0, numhapd=0, numhap=0;

int maxsni, abulbounce, sbulbounce, sniprod, shootrate, targbounce, scanprob, killwall, snikilmot, snikilsni, antkilant, makehappy;

int outofmemory=0;

typedef struct {
  char *name;
  //0=bounce forever, 1=no bounce, #=chance of bounce is 1-1/#
  short int abulbounce, sbulbounce;
  //Snipe production rate, snipe shoot rate (inverse), number of bounces for snipes to look ahead
  short int sniprod, shootrate, targbounce;
  //Probability of scanning before each move: 100/x
  short int scanprob;
  //Dangerous walls
  int killwall:1;
  //What kills what
  int snikilmot:1, snikilsni:1, antkilant:1;
  //Make happyfaces
  int makehappy:1;
} skill;

typedef struct {
  char *name;
  //Tiles wide, high, Number of cells wide, high, number of walls missing
  short int mzx, mzy; char mzcx, mzcy; short int rmw;
  //Lives, mothers, max snipes
  char lives, mot; short int sni;
} level;

/*skill skills[4]={
{"Alpha", 1,1,150,12,0,1,1,0,0},
{"Beta", 10,1,100,10,0,1,1,1,0},
{"Gamma",10,2, 50, 8,0,0,1,1,0},
{"Delta",10,2, 25, 6,1,0,0,1,1}
};*/

skill skills[26]={
{"A", 1,1,150,25,0,1500,0,1,1,0,0},
{"B", 1,1,140,20,0,1200,0,1,1,0,0},
{"C", 1,1,130,18,0,1100,0,1,1,0,0},
{"D", 1,1,120,16,0,1000,0,1,1,0,0},
{"E", 1,1,110,15,0, 900,0,1,1,0,0},
{"F", 2,1,100,14,0, 850,0,1,1,0,0},
{"G", 5,1, 95,13,0, 800,0,1,1,1,0},
{"H", 5,1, 90,12,0, 765,0,1,1,1,1},
{"I", 5,1, 85,11,0, 735,0,1,1,1,1},
{"J", 5,1, 80,10,0, 700,0,1,1,1,1},
{"K", 7,1, 75,10,0, 665,0,1,1,1,1},
{"L", 7,1, 70, 9,0, 635,0,1,1,1,1},
{"M", 7,1, 65, 9,0, 600,0,1,1,1,0},
{"N", 7,1, 60, 9,0, 565,0,1,1,1,0},
{"O", 7,1, 55, 8,0, 515,0,1,1,1,0},
{"P",10,2, 50, 8,1, 475,0,1,1,1,0},//Gamma
{"Q",10,2, 45, 7,1, 450,0,1,1,1,1},
{"R",10,2, 42, 6,1, 425,0,1,1,1,1},
{"S",10,2, 38, 6,1, 400,0,1,1,1,1},
{"T",10,1, 35, 5,0, 375,1,1,1,1,0},
{"U",10,1, 30, 4,0, 250,1,1,1,1,1},
{"V",10,2, 25, 4,1, 210,1,1,1,1,1},
{"W",10,2, 20, 3,2, 200,1,0,1,1,1},
{"X",10,2, 16, 3,2, 175,1,0,1,1,1},
{"Y",15,3, 12, 2,3, 150,1,0,1,1,1},
{"Z",20,4,  8, 2,4, 120,1,0,0,1,1}
};

level levels[10]={
{"0",  7*12, 5*12, 7, 5,10, 35,  1,   1},
{"1", 15*12,15*12,15,15,33,  7,  3,  10},
{"2", 20*12,20*12,20,20,50,  5,  4,  50},
{"3", 23*12,23*12,23,23,53,  5,  5,  80},
{"4", 23*12,23*12,23,23,53,  4,  6, 100},
{"5", 23*12,23*12,23,23,53,  3,  7, 120},
{"6", 23*12,23*12,23,23,53,  3,  8, 150},
{"7", 23*12,23*12,23,23,53,  2, 10, 200},
{"8", 31*12,31*12,31,31,96,  2, 14, 300},
{"9", 46*12,46*12,46,46,99,  1, 28,4500}
};

skill *gameskill=&skills[15];
level *gamelevel=&levels[8];
int lastskill=15, lastlevel=8;

int sound=1;
char *soundtxt[3]={"    Sound off    ", "    Sound on     ", "Sound init failed"};
//Length, number of notes; [start, stop, pitch*10000/Hz, volume]
int sounddef0[]={1200, 3, 0, 1200, 7857727, 14, 0, 1200, 23573180, 5, 0, 1200, 39288633, 3};//{1200, 1, 0, 1200, 7857727, 64};
//int sounddef1[]={1200, 3, 0, 1200, 5886644, 30, 0, 1200, 17659931, 10, 0, 1200, 29433219, 6};
int sounddef1[]={1200, 3, 0, 1200, 5886644, 19, 0, 1200, 17659931, 6, 0, 1200, 29433219, 4};
int sounddef2[]={1400, 6, 0, 700, 8820000, 13, 700, 1400, 10488806, 11,
                          0, 700, 26460000, 4, 700, 1400, 31466420, 4,
                          0, 700, 44100000, 3, 700, 1400, 52444034, 2};
int sounddef3[]={2000, 9, 0, 800, 1102500, 100, 600, 1400, 1651883, 67, 1200, 2000, 2205000, 50,
                          0, 800, 3307500, 33, 600, 1400, 4955651, 22, 1200, 2000, 6615000, 17,
                          0, 800, 5512500, 20, 600, 1400, 8259418, 13, 1200, 2000, 11025000, 10};
//int sounddef4[]={2500, 3, 0, 900, 2205000, 100, 800, 1900, 3303767, 80, 1800, 2500, 4410000, 60};
//int sounddef4[]={2500, 3, 0, 900, 1102500, 100, 800, 1900, 1651884, 80, 1800, 2500, 2205000, 60};
//int sounddef4[]={2500, 3, 0, 900, 1102500, 100, 800, 1900, 1651884, 67, 1800, 2500, 2205000, 50};
int sounddef4[]={2500, 9, 0, 900, 4410000, 25, 800, 1900, 3303767, 33, 1800, 2500, 1102500, 100,
                          0, 900, 13230000, 8, 800, 1900, 9911301, 11, 1800, 2500, 3307500, 33,
                          0, 900, 22050000, 5, 800, 1900, 16518836, 7, 1800, 2500, 5512500, 20};
int sounddef5[]={2000, 6, 0, 2000, 2475029, 45, 0, 2000, 1750110, 63,
                          0, 2000, 7425086, 15, 0, 2000, 5250329, 21,
                          0, 2000, 12375144, 9, 0, 2000, 8750548, 13};
int sounddef6[]={4100, 15, 0, 900, 3928863, 28, 800, 1700, 2778126, 40, 1600, 2500, 2205000, 50, 2400, 3300, 3500219, 31, 3200, 4100, 4410000, 25,
                           0, 900, 11786590, 9, 800, 1700, 8334378, 13, 1600, 2500, 6615000, 17, 2400, 3300, 10500658, 10, 3200, 4100, 13230000, 8,
                           0, 900, 19644317, 6, 800, 1700, 13890630, 8, 1600, 2500, 11025000, 10, 2400, 3300, 17501097, 6, 3200, 4100, 22050000, 5};
int sounddef7[]={2000, 9, 0, 800, 11773288, 19, 600, 1400, 17640000, 13, 1200, 2000, 20977614, 11,
                          0, 800, 35319863, 6, 600, 1400, 52920000, 4, 1200, 2000, 62932841, 4,
                          0, 800, 58866438, 4, 600, 1400, 88200000, 3, 1200, 2000, 104888068, 2};
int *sounddef[8]={sounddef0, sounddef1, sounddef2, sounddef3, sounddef4, sounddef5, sounddef6, sounddef7};
//Snipe-bullet, Antisnipe-bullet, Bullet-bounce, Snipe-dead, Mot-dead, Antisnipe-dead-forever, Antisnipe-dead, Happy-dead


///
///MAIN
///

//int huh=0;

int handler(SDL_Event *);
//LRESULT CALLBACK handler(HWND, unsigned int, unsigned int, long);

SDL_Surface *scr=0;
//HINSTANCE inst;
//HWND wind;
unsigned int timer;

SDL_Surface *tiles=0;
//LPDIRECTDRAW ddraw=0;
//LPDIRECTDRAWSURFACE dscr=0, dback=0, dtiles=0;

//HWAVEOUT mysound=0;
//WAVEHDR noise[3];
char *soundwvs[8];
char *soundring;
int soundringpos;

//WAVEHDR noise[7];

int main() {
//int WINAPI WinMain(HINSTANCE i, HINSTANCE ii, char *cmd, int sh) {
  /*union {
    WNDCLASS wc;
    MSG m;
  };*/
  FILE *f;
  int ll;
  char chr[3];
  if((f=fopen("/dev/urandom", "rb"))!=0) {
    fread(&ll, sizeof(int), 1, f); fclose(f);
    seedr((unsigned int)ll);
  } else {
    seedr((unsigned int)time(0));
  }
  if((f=fopen("~.snipesettings", "rb"))!=0) {
//  if((f=fopen("Snipes.settings", "rb"))!=0) {
    if(fread(&chr, 1, 3, f)) {
//    if(fread(&ll, 1, 3, f)) {
      if(chr[0]>='A'&&chr[0]<='Z') gameskill=&skills[lastskill=chr[0]-'A'];
//      if((ll&255)>='A'&&(ll&255)<='Z') gameskill=&skills[lastskill=(ll&255)-'A']; ll>>=8;
      if(chr[1]>='0'&&chr[1]<='9') gamelevel=&levels[lastlevel=chr[1]-'0'];
//      if((ll&255)>='0'&&(ll&255)<='9') gamelevel=&levels[lastlevel=(ll&255)-'0']; ll>>=8;
      if(chr[2]>='a'&&chr[2]<='h') curmode=chr[2]-'a';
//      if((ll&255)>='a'&&(ll&255)<='h') curmode=(ll&255)-'a';
      setmodevars(curmode);
    }
    fclose(f);
  }
  if(0>SDL_Init(SDL_INIT_VIDEO)) {
    printf("SDL_Init failed: \"%s\"", SDL_GetError()); return(1); }
  atexit(SDL_Quit); //Not sure if this should do anything...
  if(!(scr=SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE|SDL_ANYFORMAT|SDL_DOUBLEBUF))) {
    printf("SDL_SetVideoMode failed: \"%s\"", SDL_GetError()); return(1); }
  //inst=i;
  //memset(&wc, 0, sizeof(wc));
  //wc.lpfnWndProc=handler;
  //wc.hInstance=inst;
  //wc.hIcon=LoadIcon(inst, "hIcon");
  //wc.hCursor=LoadCursor(inst, "hCursor");
  //wc.lpszClassName="lpszClassName";
  //if(!RegisterClass(&wc)) return(0);
  //if(!(wind=CreateWindowEx(WS_EX_TOPMOST, "lpszClassName", "Snipes 18-07-2003", WS_POPUP|WS_MAXIMIZE,//WS_OVERLAPPEDWINDOW,
  //                        CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, 0, 0, inst, 0)))
  //  return(0);
  //ShowWindow(wind, sh);
  SDL_Event m;
  int noquit=1;
  m.type=/*Init*/24; handler(&m);
  while(noquit) {
    while(noquit&&SDL_PollEvent(&m)) {
      noquit=handler(&m);
    }
    m.type=/*Timer*/25;
    handler(&m);
    SDL_Delay(1);
  }
  //while((GetMessage(&m, 0, 0, 0)+1)&~1)
  //  DispatchMessage(&m);
  if(maze) free(maze);
  if((f=fopen("~.snipesettings", "wb"))!=0) {
//  if((f=fopen("Snipes.settings", "wb"))!=0) {
    chr[0]=lastskill+'A';
//    ll=lastskill+'A';
    chr[1]=lastlevel+'0';
//    ll|=(lastlevel+'0')<<8;
    chr[2]=curmode+'a';
//    ll|=(curmode+'a')<<16;
    fwrite(&chr, 1, 3, f);
//    fwrite(&ll, 1, 3, f);
    fclose(f);
  }
  SDL_Quit();
  return(0);
  //return(m.wParam);
}

int nextt, frac91;
int lcurd[18];
int handler(SDL_Event *ev) {
//LRESULT CALLBACK handler(HWND w, unsigned int m, unsigned int wp, long lp) {
  char *er; int curd, curt, x;
  //RECT r; int n;
  switch(ev->type) {
    case /*Init*/24: memset(keyb, 0, sizeof(keyb)); memset(odisp, 255, sizeof(odisp)); drawtiles(); break;
    case SDL_QUIT: return(0);
    case /*Timer*/25: if(!working) break; curd=(nextt-(curt=SDL_GetTicks()))*91+frac91; if(curd>0) return(0); 
                   if(curd<-5000) { nextt=curt; frac91=0; } else { nextt+=54; frac91+=86; if(frac91>=91) { ++nextt; frac91-=91; }}
      for(curt=16;curt>=0;--curt) lcurd[curt+1]=lcurd[curt]; lcurd[0]=curd;/*for(n=0;n<48000;++n) {
        r.left=rnd()%(15*tsx); r.right=r.left+tsx; r.top=rnd()%(15*tsy); r.bottom=r.top+tsy;
        dback->BltFast(huh?256:rnd()%632, 0?176:rnd()%472, dtiles, &r, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
      }*/
//      if(newmode>=0) if(switchmode(newmode)) { working=0; PostMessage(wind, WM_CLOSE, 0, 0); break; } else newmode=-1;
//      if(dtiles->IsLost()) { dtiles->Restore(); drawtiles(); }
//      if(dscr->IsLost()) { dscr->Restore(); bdisp=-2; }
      tickgame(); drawdisp(); SDL_Flip(scr);
      //tickgame(); //if(!dscr->IsLost()) { drawdisp(); if(!dscr->Flip(0, DDFLIP_WAIT)) bdisp+=bdisp==1?-1:1; playsounds(); }
      for(x=0;x<256;++x) keyb[x]&=~0x0C; break;

//  switch(m) {
/*    case WM_CREATE: wind=w; x=1; if(curmode!=0) if(ddinit()) setmodevars(0); else x=0; if(x&&(er=ddinit())!=0) { MessageBox(0, er, 0, 0); return(-1); } drawtiles();
                    timer=SetTimer(wind, 1, 1, 0); nextt=timeGetTime()+1; frac91=0; return(0);
    case WM_TIMER: if(!working) break; curd=(nextt-(curt=timeGetTime()))*91+frac91; if(curd>0) return(0); 
                   if(curd<-5000) { nextt=curt; frac91=0; } else { nextt+=54; frac91+=86; if(frac91>=91) { ++nextt; frac91-=91; }}
*///      for(curt=16;curt>=0;--curt) lcurd[curt+1]=lcurd[curt]; lcurd[0]=curd;/*for(n=0;n<48000;++n) {
/*        r.left=rnd()%(15*tsx); r.right=r.left+tsx; r.top=rnd()%(15*tsy); r.bottom=r.top+tsy;
        dback->BltFast(huh?256:rnd()%632, 0?176:rnd()%472, dtiles, &r, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
      }*/
/*      if(newmode>=0) if(switchmode(newmode)) { working=0; PostMessage(wind, WM_CLOSE, 0, 0); break; } else newmode=-1;
      if(dtiles->IsLost()) { dtiles->Restore(); drawtiles(); }
      if(dscr->IsLost()) { dscr->Restore(); bdisp=-2; }
      tickgame(); if(!dscr->IsLost()) { drawdisp(); if(!dscr->Flip(0, DDFLIP_WAIT)) bdisp+=bdisp==1?-1:1; playsounds(); }
      for(x=0;x<64;++x) ((int *)keyb)[x]&=~0x0C0C0C0C; return(0);
    case MM_WOM_DONE: if(sound!=1) break;
                      x=((LPWAVEHDR)lp)->dwUser;
                      memcpy(((LPWAVEHDR)lp)->lpData, soundring+soundringpos, 400);
                      memset(soundring+soundringpos, 128, 400);
                      soundringpos=(soundringpos+400)%25200;
                      waveOutWrite(mysound, &noise[x], sizeof(noise[x])); break;
    case WM_KEYDOWN: x=(lp>>16)&511; if(!(keyb[x&255]&(1<<(x>>8)))) keyb[x&255]|= (5<<(x>>8)); return(0);
    case WM_KEYUP:   x=(lp>>16)&511;                                keyb[x&255]&=~(1<<(x>>8)); return(0);
    case WM_CLOSE: working=0; KillTimer(wind, timer); ShowWindow(wind, SW_HIDE); snduninit(); dduninit(); DestroyWindow(wind); return(0);
    case WM_DESTROY: working=0; PostQuitMessage(0); break;
*/  }
  return(1);
  //return(DefWindowProc(w, m, wp, lp));
}

char *ddinit() {
/*  DDSURFACEDESC sd;
  if(DirectDrawCreate(0, &ddraw, 0)) { return("DirectDrawCreate failed."); }
  if(ddraw->SetCooperativeLevel(wind, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN)) { dduninit(); return("SetCooperativeLevel failed."); }
  if(ddraw->SetDisplayMode(resx, resy, 16)) { dduninit(); return("SetDisplayMode failed."); }
  sd.dwSize=sizeof(sd);
  sd.dwFlags=DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
  sd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE|DDSCAPS_FLIP|DDSCAPS_COMPLEX;
  sd.dwBackBufferCount=1;
  if(ddraw->CreateSurface(&sd, &dscr, 0)) { dduninit(); return("CreateSurface (screen) failed."); }
  sd.ddsCaps.dwCaps=DDSCAPS_BACKBUFFER;
  if(dscr->GetAttachedSurface(&sd.ddsCaps, &dback)) { dduninit(); return("GetAttachedSurface failed."); }
  sd.dwFlags=DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH;
  sd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN;
  sd.dwWidth=16*tsx;
  sd.dwHeight=16*tsy;
  if(ddraw->CreateSurface(&sd, &dtiles, 0)) { dduninit(); return("CreateSurface (tiles) failed."); }
//  if() { dduninit(); return(""); }
  bdisp=-2; return(0);*/
}

void dduninit() {
/*  if(dtiles) dtiles->Release(); dtiles=0;
  //if(dback) dback->Release();
  dback=0;
  if(dscr) dscr->Release(); dscr=0;
  if(ddraw) ddraw->Release(); ddraw=0;*/
}

void sndinit() {
/*  union {
    WAVEFORMATEX wf;
    int x;
  };

//HWAVEOUT mysound=0;
//WAVEHDR noise[3];
//char *soundwvs[8];
//char *soundring;
//int soundringpos;

  int y, z, m;
  char *s;
  memset(&wf, 0, sizeof(wf));
  wf.wFormatTag=WAVE_FORMAT_PCM;
  wf.nChannels=1;
  wf.nSamplesPerSec=22050;
  wf.nAvgBytesPerSec=22050;
  wf.nBlockAlign=1;
  wf.wBitsPerSample=8;
  if(waveOutOpen(&mysound, WAVE_MAPPER, &wf, (unsigned long)wind, 0, CALLBACK_WINDOW)) { sound=2; mysound=0; return; }
  for(x=0;x<8;++x) {
    if(!(soundwvs[x]=(char *)malloc(sounddef[x][0]))) {
      for(--x;x>=0;--x) free(soundwvs[x]); 
      waveOutReset(mysound); waveOutClose(mysound);
      sound=2; mysound=0; return;
    }
  }
  for(x=0;x<8;++x) {
    s=soundwvs[x];//noise[x].dwLoops=-1; noise[x].dwFlags=WHDR_BEGINLOOP|WHDR_ENDLOOP;
    memset(s, 0, sounddef[x][0]);
//    int sounddef0[]={1200, 1, 0, 1200, 7857727, 255};
    //sounddef[1][4]=rndr(5886644);
    for(y=0;y<sounddef[x][1];++y) for(z=0,m=sounddef[x][3+4*y]-sounddef[x][2+4*y];z<m;++z) {
      s[sounddef[x][2+4*y]+z]+=(char)(sounddef[x][5+4*y]*(1-z/(double)m)*sin(sounddef[x][4+4*y]*(3.14159265358979323*2./22050./10000.)*z));
    }
  }
  for(x=0;x<3;++x) {
    memset(noise+x, 0, sizeof(noise[x]));
    noise[x].dwBufferLength=400;
    noise[x].dwUser=(x+2)%3;
    if(!(noise[x].lpData=(char *)malloc(400))) {
      for(--x;x>=0;--x) free(noise[x].lpData); 
      for(x=0;x<8;++x) free(soundwvs[x]); 
      waveOutReset(mysound); waveOutClose(mysound);
      sound=2; mysound=0; return;
    }
    memset(noise[x].lpData, 128, 400);
  }
  for(x=0;x<3;++x) {
    if(waveOutPrepareHeader(mysound, &noise[x], sizeof(noise[x]))) {
      for(--x;x>=0;--x) waveOutUnprepareHeader(mysound, &noise[x], sizeof(noise[x]));
      for(x=0;x<3;++x) free(noise[x].lpData);
      for(x=0;x<8;++x) free(soundwvs[x]); 
      waveOutReset(mysound); waveOutClose(mysound);
      sound=2; mysound=0; return;
    }
  }
  if(!(soundring=(char *)malloc(25200))) {
      for(x=0;x<3;++x) waveOutUnprepareHeader(mysound, &noise[x], sizeof(noise[x]));
      for(x=0;x<3;++x) free(noise[x].lpData);
      for(x=0;x<8;++x) free(soundwvs[x]); 
      waveOutReset(mysound); waveOutClose(mysound);
      sound=2; mysound=0; return;
  }
  memset(soundring, 128, 25200);
  soundringpos=0;
  //waveOutSetVolume(mysound, ~0);
  waveOutWrite(mysound, &noise[0], sizeof(noise[x]));
  waveOutWrite(mysound, &noise[1], sizeof(noise[x]));*/
}

void snduninit() {
  /*int x;
  if(!mysound) return;
  waveOutReset(mysound);
  for(x=0;x<3;++x) {
    if(x==0) waveOutUnprepareHeader(mysound, &noise[x], sizeof(noise[x]));
    free(noise[x].lpData);
  }
  for(x=0;x<8;++x) free(soundwvs[x]);
  free(soundring);
  waveOutClose(mysound);
  mysound=0;*/
}

//
//SOUND
//

void playsounds() {
/*  int x; int y, z, t, n;//, z, m; char *s;
  if(sound==0&&mysound) { psound=0; snduninit(); }
  if(sound==1&&!mysound) sndinit();
  if(sound!=1) return;
//psound=gametime%50?0:3;*/
 /*  for(x=0;x<7;++x) {
    s=noise[x].lpData;
    memset(s, 128, noise[x].dwBufferLength);
//    int sounddef0[]={1200, 1, 0, 1200, 7857727, 255};
    sounddef[1][4]=rndr(5886644);
    for(y=0;y<sounddef[x][1];++y) for(z=0,m=sounddef[x][3+4*y]-sounddef[x][2+4*y];z<m;++z) {
      s[sounddef[x][2+4*y]+z]+=(char)(sounddef[x][5+4*y]*(1-z/(double)m)*sin(sounddef[x][4+4*y]*(3.14159265358979323*2./22050./10000.)*z));
    }
  }*/
/*
  for(x=0;x<8;++x) if(psound&(1<<x)) {
    for(y=0;y<sounddef[x][0];++y) {
      t=sounddef[x][0]-y;
      t=t<25200-soundringpos?t:25200-soundringpos;
      for(z=(soundringpos+y)%25200,t+=z;z<t;++y,++z) {
        n=(unsigned char)soundring[z]-128; n+=soundwvs[x][y];
        //soundring[z]+=soundwvs[x][y];
        soundring[z]=n>127?255:(n<-128?0:n+128);
        //gamelives=n>gamelives?n:gamelives;
        //soundring[(soundringpos+y)%25200]+=soundwvs[x][y];
      }
    }
    //waveOutWrite(mysound, &noise[x], sizeof(noise[x]));
  }
  //for(x=6;x>=0;--x) if(psound&(1<<x)) waveOutWrite(mysound, &noise[x], sizeof(noise[x]));
  psound=0;*/
}

//
//GRAPHICS
//

unsigned char *tilebit=0;//[16*16*40*40*4/*tsx*tsy*bpp*/];

void drawtiles() {
//  DDSURFACEDESC ds;
  int x, y, a, b, n, tsxp;
  unsigned short fg, bg;
  if(tilebit) free(tilebit);
  if(!(tilebit=(unsigned char *)malloc(16*16*tsx*tsy*bpp))) return;
  tsxp=tsx+(tsx&1);
  fg=colsv[0]; bg=colsv[1]; n=0;
  for(b=0;b<16;++b) for(a=0;a<16;++a) {
    if((a|(b<<4))>colsc[n]) {
      fg=colsv[(++n)<<1]; bg=colsv[(n<<1)|1];
    }
    for(y=b*tsy;y<(b+1)*tsy;++y) for(x=a*tsx;x<(a+1)*tsx;++x)
      ((unsigned short *)tilebit)[x+y*16*tsx]=(bitm[(x>>3)+(16*tsy-1-y)*2*tsxp]&(1<<(7^x&7)))?fg:bg;
  }
  //for(y=0;y<16*tsy;++y) for(x=0;x<16*tsx;++x) ((unsigned short *)tilebit)[x+y*16*tsx]=rnd();
  //for(y=0;y<tsy;++y) for(x=0;x<16*tsx;++x) ((unsigned short *)tilebit)[x+y*16*tsx]=31;///*31*  /*1<<(x/tsx);
  //for(int z=1;z<16;++z) {
  //  if(!(z&1)) for(y=0;y<tsy;++y) ((unsigned short *)tilebit)[tsx-2+z*tsx+y*16*tsx]=65535;
  //  if(!(z&2)) for(y=0;y<tsy;++y) ((unsigned short *)tilebit)[y+z*tsx+(tsy-2)*16*tsx]=65535;
  //  if(!(z&4)) for(y=0;y<tsy;++y) ((unsigned short *)tilebit)[1+z*tsx+y*16*tsx]=65535;
  //  if(!(z&8)) for(y=0;y<tsy;++y) ((unsigned short *)tilebit)[y+z*tsx+16*tsx]=65535;
  //}
  if(tiles) SDL_FreeSurface(tiles);
  tiles=SDL_CreateRGBSurfaceFrom(tilebit, 16*tsx, 16*tsy, 2*8, 16*tsx*bpp, 31<<11, 63<<5, 31, 0);
  //ds.dwSize=sizeof(ds);
  //if(dtiles->Lock(0, &ds, DDLOCK_WRITEONLY|DDLOCK_WAIT, 0)) { free(tilebit); return; } //{ huh=1; return; }
  //if(!(ds.dwFlags&DDSD_PITCH)) ds.lPitch=16*tsx*bpp;
  //for(y=0;y<16*tsy;++y) memcpy(((char *)ds.lpSurface)+y*ds.lPitch, tilebit+y*16*tsx*bpp, 16*tsx*bpp);
  //dtiles->Unlock(ds.lpSurface);
  //free(tilebit);
}

void drawdisp() {
  SDL_Rect f, t;
//  union { RECT r; DDBLTFX fx; };
  int x, y;
  unsigned char *c=disp;
//  if(bdisp<0) {
//    memset(&fx, 0, sizeof(fx)); fx.dwSize=sizeof(fx); fx.dwFillColor=0;
//    dback->Blt(0, 0, 0, DDBLT_COLORFILL, &fx);
//  }
  for(y=0;y<60;++y) for(x=0;x<80;++x,++c) if(bdisp<0||odisp[bdisp][x+y*80]!=*c/*disp[x+y*80]*/) {
    //odisp[bdisp&1][x+y*80]=*c;//disp[x+y*80];
    f.w=t.w=tsx; f.h=t.h=tsy;
    f.x=((*c)&15)*tsx;
    f.y=((*c)>>4)*tsy;
    t.x=x*tsx+tox;
    t.y=y*tsy+toy;
//    r.left=((*c)&15)*tsx;
//    r.top=((*c)>>4)*tsy;
//    r.right=r.left+tsx;
//    r.bottom=r.top+tsy;
    if(tiles&&0<=SDL_BlitSurface(tiles, &f, scr, &t))
//    if(!dback->BltFast(x*tsx+tox, y*tsy+toy, dtiles, &r, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT))
      odisp[bdisp&1][x+y*80]=*c;
    else
      odisp[bdisp&1][x+y*80]=255;
  }
}

void setmodevars(int mode) {
  curmode=mode;
  tsx=modes[curmode].tx; tsy=modes[curmode].ty; tox=modes[curmode].ox; toy=modes[curmode].oy;
  resx=modes[curmode].x; resy=modes[curmode].y;
  bitm=modes[curmode].bitm;
}

int switchmode(int nmode) {
  dduninit();
  setmodevars(nmode);
  if(ddinit()) {
    setmodevars(0);
    if(ddinit()) return(1);
  }
  drawtiles();
  return(0);
}

//
//RANDOMITY
//

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

//
// GAME
//

typedef struct {
  int x, y2, last;
  int y, next;
  char id, a, b, d;
  int x2;
} object;

object *objs=0;
int mobj=0;

int ofx, ofy;

unsigned char blank[]="\x00\x00\x00\x00\x00\x00\x00\x00";
unsigned char pant0[]="\x14\x15\x16\x1a\x1b\x1c\x1d\x1e\x1f";
unsigned char pant1[]="\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f";
unsigned char pantn[]="\x03\x07\x06\x0b\x1b\x0e\x09\x0d\x0c";
unsigned char pmot0[]="\x10\x11\x12\x13";
unsigned char pmot1[]="\x7f\x7f\x7f\x7f";
unsigned char undef[9];

int newskill=-1, newlevel=-1;

int debug=0, chngm=0, chngkd;

unsigned char sklvtxt[]="          ";

void drawgame() {
  int x, y, r;
  if(debug&2) { if(!keyb[88]) debug^=2; } else if(keyb[88]) debug^=3;
  if(chngm&2) { if(!keyb[66]) chngm^=2; } else if(keyb[66]) { chngm^=3; newmode=~curmode; chngkd=0; }
  if(keyb[59]) sound=0; if(keyb[60]) sound=1;
  //for(x=0;x<80*4;++x) disp[x]=32;
  //for(x=0;x<16;++x) disp[x]=x;
  memcpy(disp, " \x10\x11 dead =       | \x80\x88 dead =       | \x00\x81 dead =       | Skill Level  =           "
               " \x12\x13 left =       | \x80\x88 left =       | \x00\x81 left =       | Elapsed Time =           "
               "                                                                                "
               "Men left =                        SNIPES 2003!         Game Score   =     000000", 320);
  y=nummotd  ; x= 15; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  y=nummot   ; x= 95; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  y=numsnid  ; x= 33; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  y=numsni   ; x=113; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  y=numhapd  ; x= 51; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  y=numhap   ; x=131; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  y=gametime ; x=159; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  y=gamelives; x=255; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  y=gamescore; x=319; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  memcpy(disp+70, sklvtxt, 10);
  if(debug) {
    memcpy(disp+160, "Frame timing error =   ms | Key scan code =     |                               ", 80);
    for(x=y=0;x<18;++x) if(y<-lcurd[x]) y=-lcurd[x]; y/=91; x=182; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
    for(y=255;y>=0&&!keyb[y];--y); x=206; do { disp[x]=48+(y%10); y/=10; --x; } while(y);
  } else memcpy(disp+192, soundtxt[sound], 17);
  if(outofmemory) memcpy(disp+80*3+33, "OUT OF MEMORY!", 14);
  //else memcpy(disp+80*3+34, "SNIPES 2003!", 12);
  //for(x=80*4;x<80*60;++x) disp[x]=rnd();
  if(maze) for(y=0;y<60-4;++y) for(x=0;x<80;++x)
    if((!mzlsh)||(r=rndr(516+mzlsh*4))<mzlsh*40) disp[x+(y+4)*80]=maze[(x+mzlx)%mzx+(y+mzly)%mzy*mzx];
    else if(r<520) disp[x+(y+4)*80]=maze[(x+mzlx2)%mzx+(y+mzly2)%mzy*mzx];
    else disp[x+(y+4)*80]=156+(r&3)-((r-520)/12)*4;
  else for(y=0;y<60-4;++y) for(x=0;x<80;++x)
    disp[x+(y+4)*80]=15;
  x=4720;
  if((gamestate&12)==4) {
    memcpy(disp+x, "Enter new skill level (A-Z)(0-9)     ", 37);
    if(newskill>=0) disp[x+33]=newskill+'A';
    if(newlevel>=0) disp[x+34]=newlevel+'0';
    x-=80;
  }
  if((gamestate+1)&~1) {
    memcpy(disp+x, "Play another game? (Y or N)    ", 31);
    if(gamestate&256) if(gamestate&512) disp[x+28]='N'; else disp[x+28]='Y';
    x-=80;
  }
  if((gamestate&3)==1) { memcpy(disp+x, "The SNIPES have triumphed!!!  Long live the SNIPES of the Maze.  ", 65); x-=80; }
  if((gamestate&3)==2) { memcpy(disp+x, "Congratulations --- YOU ACTUALLY WON!!!  ", 41); x-=80; }
  if(chngm) {
    for(y=1635,x=0;x<8;++x,y+=80) memcpy(disp+y, modes[x].n, 10);
    disp[1635+(~newmode)*80]='*';
    if(!chngkd) {
      if(keyb[72]          ) { chngkd=1; newmode=~(((~newmode)+7)%8); }
      if(keyb[76]||keyb[80]) { chngkd=1; newmode=~(((~newmode)+1)%8); }
    } else if(!(keyb[72]||keyb[75]||keyb[80])) chngkd=0;
    if(keyb57()) { chngm=0; if(curmode!=~newmode) newmode=~newmode; }
  }
}

int lkx=0, lky=0;
void tickgame() {
 int x;
  if(gamestate==-1) initgame();
  //if(!maze) return;
  lkx=(keyb57()?2:1)*((keyb[71]||keyb[75]||keyb[79]?-1:0)+(keyb[73]||keyb[77]||keyb[81]?1:0));
  lky=(keyb57()?2:1)*((keyb[71]||keyb[72]||keyb[73]?-1:0)+(keyb[76]||keyb[79]||keyb[80]||keyb[81]?1:0));

//  lkx=(keyb[57]?2:1)*((keyb[75]?-1:0)+(keyb[77]?1:0));
  mzlx=(mzlx+lkx+mzx)%mzx; //if(rndr(100)==0) lkx=rndr(3)*2-2;
//  lky=(keyb[57]?2:1)*((keyb[72]?-1:0)+(keyb[80]?1:0));
  mzly=(mzly+lky+mzy)%mzy; //if(rndr(100)==0) lky=rndr(3)*2-2;
  mzlsh=0;
  //if(rndr(200)==0) initgame();
  //if(rndr(200)==0) drawtiles();
  if(maze&&objs)
    tickobj();
  drawgame();

  if((!gamestate)&&(keyb[1]||(keyb[29]&&keyb[70]))) gamestate=16;
  if((gamestate+1)&~1) {
    if(!(gamestate&4)) if(keyb[21]) { gamestate|=256; gamestate&=~512; } else if(keyb[49]) gamestate|=768;
    if((!(gamestate&4))&&(gamestate&256)&&keyb[28])
      if(gamestate&512) /*PostMessage(wind, WM_CLOSE, 0, 0)*/; else gamestate|=12;
    if((gamestate&8)&&!keyb[28]) { gamestate&=~8; newskill=newlevel=-1; } else if((gamestate&12)==4&&keyb[28]) gamestate=-1;
  if((gamestate&12)==4&&(x=checkkey())) if(x>='0'&&x<='9') newlevel=x-'0'; else if(x>='a'&&x<='z') newskill=x-'a';
   // if(keyb[21]) gamestate=-1; else if(keyb[49]) PostMessage(wind, WM_CLOSE, 0, 0);
  }
}

char keycodes[]="1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./";

int checkkey() {
 int x;
  for(x= 2;x<=13;++x) if(keyb[x]) return(keycodes[x-2]);
  for(x=16;x<=27;++x) if(keyb[x]) return(keycodes[x-4]);
  for(x=30;x<=40;++x) if(keyb[x]) return(keycodes[x-6]);
  for(x=44;x<=53;++x) if(keyb[x]) return(keycodes[x-9]);
  return(0);
}

void initgame() {
  int x, y, o;
  if(maze) free(maze); maze=0;
  if(objs) free(objs); objs=0; mobj=0;
  if(newskill>=0) gameskill=skills+(lastskill=newskill);
  if(newlevel>=0) gamelevel=levels+(lastlevel=newlevel);
  mzx=gamelevel->mzx; mzy=gamelevel->mzy; mzcx=gamelevel->mzcx; mzcy=gamelevel->mzcy; rmw=gamelevel->rmw;
  ofx=mzx-((38+(rnd()&1))%mzx);
  ofy=mzy-(26%mzy);
  maze=(unsigned char *)malloc(mzx*mzy);
  if(!maze) { outofmemory=1; return; }
  smobj(120);
  initmaze();
  //gametime=0;
  gametime=gametime2=gamescore=/*gamelives=*/nummotd=nummot=numsnid=numsni=numhapd=numhap=0;
  gamelives=gamelevel->lives;
  maxsni=gamelevel->sni;
  abulbounce=gameskill->abulbounce;
  sbulbounce=gameskill->sbulbounce;
  sniprod=gameskill->sniprod;
  shootrate=gameskill->shootrate;
  targbounce=gameskill->targbounce;
  scanprob=gameskill->scanprob;
  killwall=gameskill->killwall?1:0;
  snikilmot=gameskill->snikilmot?12:4;
  snikilsni=gameskill->snikilsni?164:162;
  antkilant=gameskill->antkilant?12:8;
  makehappy=gameskill->makehappy;
  for(y=0;gamelevel->name[y];++y);
  for(x=9,--y;x>=0&&y>=0;--x,--y) sklvtxt[x]=gamelevel->name[y];
  for(y=0;gameskill->name[y];++y);
  for(--y;x>=0&&y>=0;--x,--y) sklvtxt[x]=gameskill->name[y];
  for(;x>=0;--x) sklvtxt[x]=32;
  if(!(o=nobj(0))) return;
  objs[o].id=1;
  objs[o].a=0;
  objs[o].b=0;
  objs[o].d=-1;
  placeobj(o, 3, 3, pant0);
  for(x=0;x<gamelevel->mot;++x) {
    if(!(o=nobj(0))) return;
    objs[o].id=2;
    objs[o].b=0;
    placeobj(o, 2, 2, pmot0);
  }
  gamestate=0;
  outofmemory=0;
}

int dirs[5]={0, 1, 0, -1, 0};

void initmaze() {
  int x, y, z, d, x2, y2;
  for(y=0;y<mzy;++y) for(x=0;x<mzx;++x) maze[x+y*mzx]=0;
  for(y=0;y<mzcy;++y) for(x=0;x<mzcx;++x) maze[mzx*x/mzcx+mzy*y/mzcy*mzx]=63;
  maze[0]=15;
  maze[mzx*(1%mzcx)/mzcx]&=~32;
  maze[mzy*(1%mzcy)/mzcy*mzx]&=~32;
  maze[mzx*(mzcx-1)/mzcx]&=~32;
  maze[mzy*(mzcy-1)/mzcy*mzx]&=~32;

  for(z=mzcx*mzcy-1;z;--z) {
    do { x=rndr(mzcx); y=rndr(mzcy); }
      while((maze[mzx*(x%mzcx)/mzcx+mzy*(y%mzcy)/mzcy*mzx]>>4)!=1);
    do { d=rnd()&3; x2=(x+dirs[d+1]+mzcx)%mzcx; y2=(y+dirs[d]+mzcy)%mzcy; }
      while(maze[mzx*(x2%mzcx)/mzcx+mzy*(y2%mzcy)/mzcy*mzx]>>4);
    maze[mzx*x/mzcx+mzy*y/mzcy*mzx]&=~48;
    maze[mzx*((x+1)%mzcx)/mzcx+mzy*y/mzcy*mzx]&=~32;
    maze[mzx*x/mzcx+mzy*((y+1)%mzcy)/mzcy*mzx]&=~32;
    maze[mzx*((x+mzcx-1)%mzcx)/mzcx+mzy*y/mzcy*mzx]&=~32;
    maze[mzx*x/mzcx+mzy*((y+mzcy-1)%mzcy)/mzcy*mzx]&=~32;
    if(d<2) { x=x2; y=y2; }
    if(d&1) {
      maze[mzx*x/mzcx+mzy*y/mzcy*mzx]&=~1;
      maze[mzx*((x+1)%mzcx)/mzcx+mzy*y/mzcy*mzx]&=~4;
    } else {
      maze[mzx*x/mzcx+mzy*y/mzcy*mzx]&=~2;
      maze[mzx*x/mzcx+mzy*((y+1)%mzcy)/mzcy*mzx]&=~8;
    }
  }
  for(z=0;z<rmw;++z) {
    do { x=rndr(mzcx); y=rndr(mzcy); d=(rnd()&1)+1; }
      while((maze[mzx*(x%mzcx)/mzcx+mzy*(y%mzcy)/mzcy*mzx]&d)==0);
    if(d==1) {
      maze[mzx*x/mzcx+mzy*y/mzcy*mzx]&=~1;
      maze[mzx*((x+1)%mzcx)/mzcx+mzy*y/mzcy*mzx]&=~4;
    } else {
      maze[mzx*x/mzcx+mzy*y/mzcy*mzx]&=~2;
      maze[mzx*x/mzcx+mzy*((y+1)%mzcy)/mzcy*mzx]&=~8;
    }
  }
  for(y=y2=0;y2<mzcy;++y2,y=mzy*y2/mzcy) for(x=d=0;x<mzx;++x)
    if(maze[x+y*mzx]) d=maze[x+y*mzx]&1; else if(d) maze[x+y*mzx]=5;
  for(x=x2=0;x2<mzcx;++x2,x=mzx*x2/mzcx) for(y=d=0;y<mzy;++y)
    if(maze[x+y*mzx]) d=maze[x+y*mzx]&2; else if(d) maze[x+y*mzx]=10;
  mzlx=rndr(mzx);
  mzly=rndr(mzy);
}

//object *objs=0;
//int mobj=0;

void smobj(int m) {
  object *nobjs;
  int x;
  if(m<=mobj) return; if(m<3) m=3;
  if(!(nobjs=(object *)malloc(m*sizeof(object)))) return;
  if(objs==0) {
    nobjs[0].last=0; nobjs[0].next=0;
    nobjs[1].last=1; nobjs[1].next=1;
    mobj=2;
  } else memcpy(nobjs, objs, mobj*sizeof(object));
  objs=nobjs;
  objs[mobj].last=objs[1].last;
  objs[objs[1].last].next=mobj;
  objs[m-1].next=1;
  objs[1].last=m-1;
  for(x=mobj;x<m-1;++x) {
    objs[x].next=x+1;
    objs[x+1].last=x;
  }
  mobj=m;
}

int nobj(int b) {
  int o;
  if((o=objs[1].next)==1) { smobj(mobj>1000000000?2000000000:mobj*2);
    if((o=objs[1].next)==1) { outofmemory=1; return(0); }
  }
  objs[objs[b].last].next=o;
  objs[o].last=objs[b].last;
  objs[objs[o].next].last=1;
  objs[1].next=objs[o].next;
  objs[o].next=b;
  objs[b].last=o;
  return(o);
}

void kobj(int o) {
  objs[objs[o].last].next=objs[o].next;
  objs[objs[o].next].last=objs[o].last;
  objs[objs[1].next].last=o;
  objs[o].next=objs[1].next;
  objs[o].last=1;
  objs[1].next=o;
}

void placeobj(int o, int sx, int sy, unsigned char *p) {
  int x, y, a, b;
  z:
  x=rndr(mzx); y=rndr(mzy);
  for(b=y;b<y+sy;++b) for(a=x;a<x+sx;++a) if(maze[(a%mzx)+(b%mzy)*mzx]) goto z;
  objs[o].x=x; objs[o].y=y;
  for(b=y;b<y+sy;++b) for(a=x;a<x+sx;++a) maze[(a%mzx)+(b%mzy)*mzx]=*(p++);
}

void drawobj(int o, int sx, int sy, unsigned char *p) {
  int x, y, a, b;
  x=objs[o].x; y=objs[o].y;
  for(b=y;b<y+sy;++b) for(a=x;a<x+sx;++a) maze[(a%mzx)+(b%mzy)*mzx]=*(p++);
}

int checkobj(int x, int y, int sx, int sy) {
  int a, b, r=0, t;
  for(b=y;b<y+sy;++b) for(a=x;a<x+sx;++a) { t=maze[(a%mzx)+(b%mzy)*mzx]; r|=(t?1:0)|(t&&t<16?2:0)|(t>=160&&t<162?4:0)|(t>=162&&t<164?8:0); }
  return(r);
}

int trace(int *xp, int *yp, int dx, int dy, int *dst) {
 int d=dx+dy*mzx, c, n, ds, i, x=*xp, y=*yp;
  ds=*dst;
  while(ds) {
    x=(x+dx+mzx)%mzx; y=(y+dy+mzy)%mzy; --ds;
    n=dx?(dx<0?x/-dx:(mzx-x-1)/dx):0x7fffffff;
    c=dy?(dy<0?y/-dy:(mzy-y-1)/dy):0x7fffffff;
    n=n<c?n:c; n=n<ds?n:ds;
    for(c=x+y*mzx,i=0;i<=n;++i,c+=d) if(maze[c]) {
      *dst=ds-i;
      *xp=x+i*dx; *yp=y+i*dy;
      return(maze[c]);
    }
    ds-=n;
    x+=n*dx; y+=n*dy;
  }
  *dst=0;
  *xp=x; *yp=y;
  return(0);
}

//int dirx[8]={      1, 1, 0, -1, -1, -1, 0, 1};
//int diry[8]={0, 1, 1, 1, 0, -1, -1, -1
int dirxy[10]={0, 1, 1, 1, 0, -1, -1, -1, 0, 1};
#define dirx (dirxy+2)
#define diry (dirxy)
int xydir[9]={5, 6, 7, 4, -1, 0, 3, 2, 1};

//int mdirx[12]={         2, 2, 2, 1, 0, -1, -1, -1, -1, 0, 1, 2};
//int mdiry[12]={0, 1, 2, 2, 2, 2, 1, 0, -1, -1, -1, -1         };
int  mdirxy[15]={0, 1, 2, 2, 2, 2, 1, 0, -1, -1, -1, -1, 0, 1, 2};

void tickobj() {
  int o, no, co;
  int dx, dy, x, y, x2, y2, /*x3, y3, x4, y4,*/ d/*, ch*/;
  int lnumsni;
  if(!objs) return;
  lnumsni=numsni+numhap;
  nummot=numsni=numhap=0;
  for(o=objs[0].next;o;o=no) {
    no=objs[o].next;
    switch(objs[o].id) {
      case   0: //Deleteme
                kobj(o); break;
      case   1: //Antisnipe
                if((!objs[o].b)&&(d=checkobj(objs[o].x, objs[o].y, 3, 3)&antkilant)) {
                  killedbywall:
                  objs[o].b=13;
                  if(!(objs[o].a=!--gamelives)) {
                    x=objs[o].x; y=objs[o].y;
                    placeobj(o, 3, 3, pantn);
                    objs[o].x2=objs[o].x; objs[o].y2=objs[o].y;
                    objs[o].x=x; objs[o].y=y;
                    psound|=64;
                  } else { psound|=32; if(!gamestate) gamestate=1; }
                }
                if(objs[o].b) {
                  if(!(--objs[o].b)) if(objs[o].a) {
                    objs[o].id=0;
                    drawobj(o, 3, 3, blank);
                    break;
                  } else {
                    drawobj(o, 3, 3, blank);
                    objs[o].x=objs[o].x2; objs[o].y=objs[o].y2;
                    mzlx=(objs[o].x+ofx)%mzx;
                    mzly=(objs[o].y+ofy)%mzy;
                    break;
                  }
                  for(x=0;x<9;++x) undef[x]=144+(((12-objs[o].b)/3)<<2)+(rnd()&3);
                  drawobj(o, 3, 3, undef);
                  mzlx=(objs[o].x+ofx)%mzx;
                  mzly=(objs[o].y+ofy)%mzy;
                  if(!objs[o].a) {
                    mzlx2=(objs[o].x2+ofx)%mzx;
                    mzly2=(objs[o].y2+ofy)%mzy;
                    mzlsh=objs[o].b;
                  }
                  break;
                }
                drawobj(o, 3, 3, blank);
                dx=(keyb[71]||keyb[75]||keyb[79]?-1:0)+(keyb[73]||keyb[77]||keyb[81]?1:0);
                dy=(keyb[71]||keyb[72]||keyb[73]?-1:0)+(keyb[76]||keyb[79]||keyb[80]||keyb[81]?1:0);
                x=(objs[o].x+dx+mzx)%mzx;
                y=(objs[o].y+dy+mzy)%mzy;
                if(!checkobj(x, y, 3, 3)) {
                  objs[o].x=x; objs[o].y=y;
                } else if(killwall) goto killedbywall; else if(rnd()&1)
                  { if(!checkobj(objs[o].x, y, 3, 3)) objs[o].y=y; else if(!checkobj(x, objs[o].y, 3, 3)) objs[o].x=x; }
                else
                  { if(!checkobj(x, objs[o].y, 3, 3)) objs[o].x=x; else if(!checkobj(objs[o].x, y, 3, 3)) objs[o].y=y; }
                if(keyb57()) {
                  x=(objs[o].x+dx+mzx)%mzx;
                  y=(objs[o].y+dy+mzy)%mzy;
                  if(!checkobj(x, y, 3, 3)) {
                    objs[o].x=x; objs[o].y=y;
                  } else if(rnd()&1)
                    { if(!checkobj(objs[o].x, y, 3, 3)) objs[o].y=y; else if(!checkobj(x, objs[o].y, 3, 3)) objs[o].x=x; }
                  else
                    { if(!checkobj(x, objs[o].y, 3, 3)) objs[o].x=x; else if(!checkobj(objs[o].x, y, 3, 3)) objs[o].y=y; }
                }
                mzlx=(objs[o].x+ofx)%mzx;
                mzly=(objs[o].y+ofy)%mzy;
                drawobj(o, 3, 3, (gametime2&8)?pant1:pant0);
                dx=(keyb[16]||keyb[30]||keyb[44]?-1:0)+(keyb[18]||keyb[32]||keyb[46]?1:0);
                dy=(keyb[16]||keyb[17]||keyb[18]?-1:0)+(keyb[31]||keyb[44]||keyb[45]||keyb[46]?1:0);
                if(objs[o].d!=xydir[4+dx+dy*3]) { objs[0].a=0; objs[o].d=xydir[4+dx+dy*3]; }
                if((!keyb57())&&(dx||dy)) if(objs[o].a) --objs[o].a; else {
                  objs[o].a=3;
                  psound|=2;
                  d=xydir[4+dx+dy*3];
                  x=(objs[o].x+1+dx*2+mzx)%mzx;
                  y=(objs[o].y+1+dy*2+mzy)%mzy;
                  if(maze[x+y*mzx]>=16) maze[x+y*mzx]=160+(rnd()&1); else if((!maze[x+y*mzx])&&(co=nobj(o))) {
                    //no=objs[o].next;
                    objs[co].id=3; objs[co].b=rnd()&1; objs[co].d=d;
                    objs[co].x=x; objs[co].y=y;
                    maze[x+y*mzx]=131^objs[co].b;
                  }
                } else objs[o].a=0;
                break;
      case   2: //Mother
                ++nummot;
                if((!objs[o].b)&&(d=checkobj(objs[o].x, objs[o].y, 2, 2)&snikilmot)) {
                  objs[o].b=5;//objs[o].id=0;
                  //drawobj(o, 2, 2, blank);
                  ++nummotd; psound|=16; if(d&4) gamescore+=500; if(gamescore>2000000000) gamescore=2000000000; //--nummot;
                  //break;
                } else if(!objs[o].b) {
                  drawobj(o, 2, 2, (gametime2&1)?pmot1:pmot0);
                  if(lnumsni<maxsni&&!rndr(sniprod)) {
                    d=rndr(12); x=(objs[o].x+mdirxy[d+3]+mzx)%mzx; y=(objs[o].y+mdirxy[d]+mzy)%mzy;
                    if(maze[x+y*mzx]==0&&(co=nobj(o))) {
                      objs[co].id=5; objs[co].a=1; objs[co].b=0; objs[co].d=rnd()&7;
                      objs[co].x=x; objs[co].y=y;
                      maze[x+y*mzx]=128;
                      ++lnumsni; ++numsni;
                    }
                  }
                  break;
                }
                --nummot;
                if(!(--objs[o].b)) {
                  objs[o].id=0;
                  drawobj(o, 2, 2, blank);
                  break;
                }
                for(x=0;x<4;++x) undef[x]=144+((4-objs[o].b)<<2)+(rnd()&3);
                drawobj(o, 2, 2, undef);
                break;
      case   3: //Antisnipe-bullet
      case   4: //Snipe-bullet
                x=x2=objs[o].x; y=y2=objs[o].y;
                if(maze[x+y*mzx]>=160) {
                  maze[x+y*mzx]=0; objs[o].id=0; break;
                }
                d=objs[o].d;
                dx=dirxy[d+2]; dy=dirxy[d];
                maze[x+y*mzx]=0;
                x=(x+dx+mzx)%mzx; y=(y+dy+mzy)%mzy;
                co=0;
                abulretr1:
                if(maze[x+y*mzx]) {
                  if(maze[x+y*mzx]>=16) {
                    maze[x+y*mzx]=160+(rnd()&1)+(objs[o].id==4?2:0);
                    objs[o].id=0;
                    break;
                  }
                  if(!(d&1)) { objs[o].id=0; break; }
                  d=(maze[x+y2*mzx]&&(maze[x+y2*mzx]<16))|((maze[x2+y*mzx]&&(maze[x2+y*mzx]<16))<<1);
                  if(d==0) if(rnd()&1) dx*=-1; else dy*=-1; else { if(d&1) dx*=-1; if(d&2) dy*=-1; }
                  objs[o].d=d=xydir[4+dx+dy*3];
                  x=(x2+dx+mzx)%mzx; y=(y2+dy+mzy)%mzy;
                  if(co||
                    (objs[o].id!=4?(abulbounce&&(abulbounce==1||!rndr(abulbounce)))
                                  :(sbulbounce&&(sbulbounce==1||!rndr(sbulbounce))))) { objs[o].id=0; break; }
                  else { psound|=4; co=1; goto abulretr1; }
                }
                x=((x2=x)+dx+mzx)%mzx; y=((y2=y)+dy+mzy)%mzy;
                co=0;
                abulretr2:
                if(maze[x+y*mzx]) {
                  if(maze[x+y*mzx]>=16) {
                    maze[x+y*mzx]=160+(rnd()&1)+(objs[o].id==4?2:0);
                    objs[o].id=0;
                    break;
                  }
                  if(!(d&1)) { objs[o].id=0; break; }
                  d=(maze[x+y2*mzx]&&(maze[x+y2*mzx]<16))|((maze[x2+y*mzx]&&(maze[x2+y*mzx]<16))<<1);
                  if(d==0) if(rnd()&1) dx*=-1; else dy*=-1; else { if(d&1) dx*=-1; if(d&2) dy*=-1; }
                  objs[o].d=d=xydir[4+dx+dy*3];
                  x=(x2+dx+mzx)%mzx; y=(y2+dy+mzy)%mzy;
                  if(co||
                    (objs[o].id!=4?(abulbounce&&(abulbounce==1||!rndr(abulbounce)))
                                  :(sbulbounce&&(sbulbounce==1||!rndr(sbulbounce))))) { objs[o].id=0; break; }
                  else { co=1; psound|=4; goto abulretr2; }
                }
                maze[x+y*mzx]=objs[o].id==4?136+d:130+objs[o].b;
                objs[o].b^=1;
                objs[o].x=x; objs[o].y=y;
                break;
      case   5: //Snipe
                ++numsni;
                x=objs[o].x; y=objs[o].y;
                d=objs[o].d;
                x2=(x+dirxy[d+2]+mzx)%mzx; y2=(y+dirxy[d]+mzy)%mzy;
                if((!(objs[o].b&1))&&((maze[x+y*mzx]>=160&&maze[x+y*mzx]<snikilsni)||((objs[o].b&2)&&maze[x2+y2*mzx]>=160&&maze[x2+y2*mzx]<snikilsni))) {
                  objs[o].a=5; objs[o].b|=1;
                  ++numsnid;
                  if((maze[x+y*mzx]>=160&&maze[x+y*mzx]<162)||((objs[o].b&2)&&(maze[x2+y2*mzx]>=160&&maze[x2+y2*mzx]<162)))
                    gamescore+=10;
                  if(makehappy&&!(maze[x+y*mzx]>=160&&maze[x+y*mzx]<snikilsni)) {
                    objs[o].id=6; objs[o].a=1; objs[o].b=0; maze[x+y*mzx]=129; maze[x2+y2*mzx]=0; ++numhap; break;
                  } else psound|=8;
                }
                if(objs[o].b&1) {
                  --numsni;
                  if(--objs[o].a) {
                    maze[x+y*mzx]=144+((4-objs[o].a)<<2)+(rnd()&3);
                    if(objs[o].b&2)
                      maze[x2+y2*mzx]=144+((4-objs[o].a)<<2)+(rnd()&3);
                  } else {
                    maze[x+y*mzx]=0;
                    if(objs[o].b&2)
                      maze[x2+y2*mzx]=0;
                    objs[o].id=0;
                  }
                  break;
                }
                maze[x+y*mzx]=128; if(objs[o].b&2) maze[x2+y2*mzx]=136+d;
                if((!(objs[o].b&2))&&!objs[o].a) {
                  if(!maze[x2+y2*mzx]) {
                    maze[x2+y2*mzx]=136+d;
                    objs[o].b|=2;
                    objs[o].a=1;
                  } else objs[o].d=rnd()&7;
                  break;
                }
                /*if((objs[o].b&2)&&!rndr(shootrate)) {
                 co=mzx>mzy?mzx:mzy; x3=x2; y3=y2;
                 while((ch=trace(&x3, &y3, dirxy[d+2], dirxy[d], &co))>=136&&ch<144);
                 if(ch&&ch<16&&(d&1)&&sbulbounce!=1) {
                   co=0;
                   x4=(x3+mzx-dirxy[d+2])%mzx; y4=(y3+mzy-dirxy[d])%mzy;
                   if(maze[x3+y4*mzx]&&maze[x3+y4*mzx]<16) co=1;
                   if(maze[x4+y3*mzx]&&maze[x4+y3*mzx]<16) co|=2;
                   //if(!rndr(1)) maze[x4+y4*mzx]=32;
                   if(!maze[x4+y4*mzx])
                     if(co==1) {
                       co=mzx>mzy?mzx:mzy;
                       while((ch=trace(&x4, &y4, -dirxy[d+2], dirxy[d], &co))>=136&&ch<144);
                     } else if(co==2) {
                       co=mzx>mzy?mzx:mzy;
                       while((ch=trace(&x4, &y4, dirxy[d+2], -dirxy[d], &co))>=136&&ch<144);
                     }
                 }
                 if((ch>=20&&ch<32)||(ch==130||ch==131)) {
                  objs[o].a=2;
                  objs[o].b&=~2;
                  x=(x2+dirxy[d+2]+mzx)%mzx;
                  y=(y2+dirxy[d]+mzy)%mzy;
                  maze[x2+y2*mzx]=0;
                  psound|=1;
                  if(maze[x+y*mzx]>=16) maze[x+y*mzx]=162+(rnd()&1); else if((!maze[x+y*mzx])&&(co=nobj(o))) {
                    //no=objs[o].next;
                    objs[co].id=4; objs[co].b=0; objs[co].d=d;
                    objs[co].x=x; objs[co].y=y;
                    maze[x+y*mzx]=136+d;
                  }
                  break;
                }}*/
                if((objs[o].b&2)&&(!rndr(shootrate))&&snitrace(x2, y2, dirxy[d+2], dirxy[d], targbounce)) {
                  objs[o].a=2;
                  objs[o].b&=~2;
                  x=(x2+dirxy[d+2]+mzx)%mzx;
                  y=(y2+dirxy[d]+mzy)%mzy;
                  maze[x2+y2*mzx]=0;
                  psound|=1;
                  if(maze[x+y*mzx]>=16) maze[x+y*mzx]=162+(rnd()&1); else if((!maze[x+y*mzx])&&(co=nobj(o))) {
                    //no=objs[o].next;
                    objs[co].id=4; objs[co].b=0; objs[co].d=d;
                    objs[co].x=x; objs[co].y=y;
                    maze[x+y*mzx]=136+d;
                  }
                  break;
                }
                /////////////////////////////////
                if((objs[o].b&2)&&!objs[o].a) {
                  if(rndr(scanprob)<100&&(co=sniscan(x2, y2, objs[o].d, targbounce))>=0) d=co;
                  else if(!rndr(7)) d=rnd()&7;
                  dx=(x2+dirxy[d+2]+mzx)%mzx; dy=(y2+dirxy[d]+mzy)%mzy;
                  while((objs[o].d^d)!=4&&maze[dx+dy*mzx]) {
                    d=rnd()&7;
                    dx=(x2+dirxy[d+2]+mzx)%mzx; dy=(y2+dirxy[d]+mzy)%mzy;
                  }
                  //if((!maze[dx+dy*mzx])||(objs[o].d^d)==4) {
                    maze[ x+ y*mzx]=0;
                    maze[x2+y2*mzx]=128;
                    maze[dx+dy*mzx]=136+d;
                    objs[o].x=x2; objs[o].y=y2;
                    objs[o].a=3; objs[o].d=d;
                  //}
                } else --objs[o].a;
                break;
      case   6: //Happy face
                ++numhap;
                x=objs[o].x; y=objs[o].y;
                //d=objs[o].d;
                //x2=(x+dirxy[d+2]+mzx)%mzx; y2=(y+dirxy[d]+mzy)%mzy;
                if((!(objs[o].b&1))&&maze[x+y*mzx]>=160&&maze[x+y*mzx]<snikilsni) {
                  objs[o].a=5; objs[o].b|=1;
                  ++numhapd;
                  if(maze[x+y*mzx]<162)
                    gamescore+=10;
                  psound|=128;
                }
                if(objs[o].b&1) {
                  --numhap;
                  if(--objs[o].a)
                    maze[x+y*mzx]=144+((4-objs[o].a)<<2)+(rnd()&3);
                  else {
                    maze[x+y*mzx]=0;
                    objs[o].id=0;
                  }
                  break;
                }
                maze[x+y*mzx]=129;
                if(objs[o].a^=1) {
                  //d=rnd()&7;
                  objs[o].d=d=hapscan(x, y, objs[o].d, 2);
                  x2=(x+dirxy[d+2]+mzx)%mzx; y2=(y+dirxy[d]+mzy)%mzy;
                  if(!maze[x2+y2*mzx]) { maze[x2+y2*mzx]=129; maze[x+y*mzx]=0; objs[o].x=x2; objs[o].y=y2; }
                }
                break;
    }
  }
  if(!(gamestate||nummot||numsni)) gamestate=2;
  if(!gamestate) ++gametime; ++gametime2;
}

//int trace(int *xp, int *yp, int dx, int dy, int *dst);
int snitrace(int xp, int yp, int dx, int dy, int bounces) {
  int dst, dsts, res, rx, ry, ax, ay;
  dst=dsts=mzx>mzy?mzx:mzy;
  do {
    if(dx&&dy&&bounces) do {
      if(((res=trace(&xp, &yp, dx, dy, &dst))>=16)||!bounces--) break;
      rx=(xp-dx+mzx)%mzx; ry=(yp-dy+mzy)%mzy;
      if(maze[rx+ry*mzx]) break;
      ax=maze[xp+ry*mzx]; ay=maze[rx+yp*mzx];
      ax=ax&&ax<16; ay=ay&&ay<16;
      if(ax) if(ay) break; else dx=-dx; else if(ay) dy=-dy; else break;
      xp=rx; yp=ry;
    } while(dst); else res=trace(&xp, &yp, dx, dy, &dst);
    if(!dst) break;
    if((res|7)!=143) break;
    ax=res&7;
    rx=(xp-dirxy[ax+2]+mzx)%mzx; ry=(yp-dirxy[ax]+mzy)%mzy;
  } while(maze[rx+ry*mzx]!=128);
  if((res|1)==131) return(dsts-dst);
  return((res<32&&res>=20)?~(dsts-dst):0);
}

int sniscan(int xp, int yp, int d, int bounces) {
  int od, a, b, cd;
  /*union {*/ int dirsdw[2]; char dirs[8]; //};
  od=rndr(40320);
  dirsdw[0]=0x00010203; dirsdw[1]=0x04050607;
  for(a=7;a>=0;--a) {
    cd=dirs[b=od%(a+1)];
    if((cd^d)==4?snitrace((xp+dirxy[cd+2]+mzx)%mzx, (yp+dirxy[cd]+mzy)%mzy, dirxy[cd+2], dirxy[cd], bounces)
                :snitrace(xp, yp, dirxy[cd+2], dirxy[cd], bounces)) return(cd);
    od/=(a+1); dirs[b]=dirs[a];
  }
  return(-1);
}

int hapscan(int xp, int yp, int d, int bounces) {
  int od, a, b, cd, r, s=-1, f=0;
  /*union {*/ int dirsdw[2]; char dirs[8]; //};
  od=rndr(40320);
  dirsdw[0]=0x00010203; dirsdw[1]=0x04050607;
  for(a=7;a>=0;--a) {
    cd=dirs[b=od%(a+1)];
    r=snitrace(xp, yp, dirxy[cd+2], dirxy[cd], bounces);
    if(r<~1) return(cd); else if(r==0) s=cd; else if(r>0) f=1;
    od/=(a+1); dirs[b]=dirs[a];
  }
  return(f||(rnd()&1)?(s==-1?rnd()&7:s):d);//s==-1?rnd()&7:s);
}

//Alternatives to spacebar: `, Tab, Capslock, Shift, Ctrl, v, b, n, m, ,, ., /, Enter, Keypad+, Keypad-, Ins, Del
int keyb57() {
  return(keyb[15]||keyb[28]||keyb[29]||keyb[41]
       ||keyb[42]||keyb[47]||keyb[48]||keyb[49]
       ||keyb[50]||keyb[51]||keyb[52]||keyb[53]
       ||keyb[54]||keyb[57]||keyb[58]||keyb[74]
       ||keyb[78]||keyb[82]||keyb[83]);
}
