#define WIN32_LEAN_AND_MEAN
#define INITGUID
#include <windows.h>
#include <mmsystem.h>
#include <malloc.h>
#include <ddraw.h>
#include <time.h>
#include <math.h>
//#include <stdio.h>
///
///ALL
///

#include "SnipesBitmaps.h"

void seedr(unsigned int);
unsigned int rnd();
unsigned int rnd(unsigned int);
char *ddinit();
void dduninit();
void sndinit();
void snduninit();
void playsounds();
void drawtiles();
void drawdisp();
void drawgame();
void tickgame();
void initgame();
void initmaze();
void smobj(int);
int nobj(int);
void kobj(int);
void placeobj(int, int, int, unsigned char *);
void drawobj(int, int, int, unsigned char *);
int checkobj(int, int, int, int);
void tickobj();

int bpp=2;
int tsx=8;
int tsy=8;
unsigned char *bitm=bitm8x8;

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
int rmw=53;
//int rmw/*=53*/;

unsigned char *maze=0;
int gametime=0, gametime2=0, gamescore=0, gamelives=0, gamestate=-1;
int nummotd=0, nummot=0, numsnid=0, numsni=0, numhapd=0, numhap=0;

int maxsni, abulbounce, sbulbounce;

int outofmemory=0;

struct skill {
  char *name;
  //0=bounce forever, 1=no bounce, #=chance of bounce is 1-1/#
  int abulbounce, sbulbounce;
};

struct level {
  char *name;
  //Tiles wide, high, Number of cells wide, high, number of walls missing
  int mzx, mzy, mzcx, mzcy, rmw;
  //Lives, mothers, max snipes
  int lives, mot, sni;
};

skill skills[1]={
{"Testing",50,2}
};

level levels[1]={
{"69", 23*12,23*12,23,23,53, 2, 14, 300}
};

skill *gameskill=skills;
level *gamelevel=levels;

int sound=1;
char *soundtxt[3]={"    Sound off    ", "    Sound on     ", "Sound init failed"};
//Length, number of notes; [start, stop, pitch*10000/Hz, volume]
int sounddef0[]={1200, 1, 0, 1200, 7857727, 127};
int sounddef1[]={12000, 3, 0, 12000, 5886644, 96, 0, 12000, 17659931, 32, 0, 12000, 29433219, 11};
int sounddef2[]={1000, 0};
int sounddef3[]={1000, 0};
int sounddef4[]={1000, 0};
int sounddef5[]={1000, 0};
int sounddef6[]={1000, 0};
int *sounddef[7]={sounddef0, sounddef1, sounddef2, sounddef3, sounddef4, sounddef5, sounddef6};

///
///MAIN
///

//int huh=0;

LRESULT CALLBACK handler(HWND, unsigned int, unsigned int, long);

HINSTANCE inst;
HWND wind;
unsigned int timer;

LPDIRECTDRAW ddraw=0;
LPDIRECTDRAWSURFACE dscr=0, dback=0, dtiles=0;

HWAVEOUT mysound=0;
WAVEHDR noise[7];

int WINAPI WinMain(HINSTANCE i, HINSTANCE ii, char *cmd, int sh) {
  seedr((unsigned int)time(0));
  union {
    WNDCLASS wc;
    MSG m;
  };
  inst=i;
  memset(&wc, 0, sizeof(wc));
  wc.lpfnWndProc=handler;
  wc.hInstance=inst;
  wc.hIcon=LoadIcon(inst, "hIcon");
  wc.hCursor=LoadCursor(inst, "hCursor");
  wc.lpszClassName="lpszClassName";
  if(!RegisterClass(&wc)) return(0);
  if(!(wind=CreateWindowEx(WS_EX_TOPMOST, "lpszClassName", "Snipes 18-07-2003", WS_POPUP|WS_MAXIMIZE,//WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, 0, 0, inst, 0)))
    return(0);
  ShowWindow(wind, sh);
  while((GetMessage(&m, 0, 0, 0)+1)&~1)
    DispatchMessage(&m);
  if(maze) free(maze);
  return(m.wParam);
}

int nextt, frac91;
int lcurd[18];
LRESULT CALLBACK handler(HWND w, unsigned int m, unsigned int wp, long lp) {
  char *er; int curd, curt, x;
  //RECT r; int n;
  switch(m) {
    case WM_CREATE: wind=w; if((er=ddinit())!=0) { MessageBox(0, er, 0, 0); return(-1); } drawtiles();
                    timer=SetTimer(wind, 1, 1, 0); nextt=timeGetTime()+1; frac91=0; return(0);
    case WM_TIMER: curd=(nextt-(curt=timeGetTime()))*91+frac91; if(curd>0) return(0); 
                   if(curd<-5000) { nextt=curt; frac91=0; } else { nextt+=54; frac91+=86; if(frac91>=91) { ++nextt; frac91-=91; }}
      for(curt=16;curt>=0;--curt) lcurd[curt+1]=lcurd[curt]; lcurd[0]=curd;/*for(n=0;n<48000;++n) {
        r.left=rnd()%(15*tsx); r.right=r.left+tsx; r.top=rnd()%(15*tsy); r.bottom=r.top+tsy;
        dback->BltFast(huh?256:rnd()%632, 0?176:rnd()%472, dtiles, &r, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
      }*/
      if(dtiles->IsLost()) { dtiles->Restore(); drawtiles(); }
      if(dscr->IsLost()) { dscr->Restore(); bdisp=-2; }
      tickgame(); if(!dscr->IsLost()) { drawdisp(); dscr->Flip(0, DDFLIP_WAIT); bdisp+=bdisp==1?-1:1; playsounds(); }
      for(x=0;x<64;++x) ((int *)keyb)[x]&=~0x0C0C0C0C; return(0);
    case WM_KEYDOWN: x=(lp>>16)&511; if(!(keyb[x&255]&(1<<(x>>8)))) keyb[x&255]|= (5<<(x>>8)); return(0);
    case WM_KEYUP:   x=(lp>>16)&511;                                keyb[x&255]&=~(1<<(x>>8)); return(0);
    case WM_CLOSE: KillTimer(wind, timer); ShowWindow(wind, SW_HIDE); snduninit(); dduninit(); DestroyWindow(wind); return(0);
    case WM_DESTROY: PostQuitMessage(0); break;
  }
  return(DefWindowProc(w, m, wp, lp));
}

char *ddinit() {
  DDSURFACEDESC sd;
  if(DirectDrawCreate(0, &ddraw, 0)) { return("DirectDrawCreate failed."); }
  if(ddraw->SetCooperativeLevel(wind, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN)) { dduninit(); return("SetCooperativeLevel failed."); }
  if(ddraw->SetDisplayMode(640, 480, 16)) { dduninit(); return("SetDisplayMode failed."); }
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
  bdisp=-2; return(0);
}

void dduninit() {
  if(dtiles) dtiles->Release(); dtiles=0;
  //if(dback) dback->Release();
  dback=0;
  if(dscr) dscr->Release(); dscr=0;
  if(ddraw) ddraw->Release(); ddraw=0;
}

void sndinit() {
  union {
    WAVEFORMATEX wf;
    int x;
  };
  int y, z, m;
  char *s;
  memset(&wf, 0, sizeof(wf));
  wf.wFormatTag=WAVE_FORMAT_PCM;
  wf.nChannels=1;
  wf.nSamplesPerSec=22050;
  wf.nAvgBytesPerSec=22050;
  wf.nBlockAlign=1;
  wf.wBitsPerSample=8;
  if(waveOutOpen(&mysound, WAVE_MAPPER, &wf, 0, 0, 0)) { sound=2; mysound=0; return; }
  for(x=0;x<7;++x) {
    memset(noise+x, 0, sizeof(noise[x]));
    if(!(noise[x].lpData=(char *)malloc(noise[x].dwBufferLength=sounddef[x][0]))) {
      for(--x;x>=0;--x) free(noise[x].lpData); 
      waveOutReset(mysound); waveOutClose(mysound);
      sound=2; mysound=0; return;
    }
  }
  for(x=0;x<7;++x) {
    s=noise[x].lpData;//noise[x].dwLoops=-1; noise[x].dwFlags=WHDR_BEGINLOOP|WHDR_ENDLOOP;
    memset(s, 128, noise[x].dwBufferLength);
//    int sounddef0[]={1200, 1, 0, 1200, 7857727, 255};
    //sounddef[1][4]=rnd(5886644);
    for(y=0;y<sounddef[x][1];++y) for(z=0,m=sounddef[x][3+4*y]-sounddef[x][2+4*y];z<m;++z) {
      s[sounddef[x][2+4*y]+z]+=(char)(sounddef[x][5+4*y]*(1-z/(double)m)*sin(sounddef[x][4+4*y]*(3.14159265358979323*2./22050./10000.)*z));
    }
  }
  for(x=0;x<7;++x) {
    if(waveOutPrepareHeader(mysound, &noise[x], sizeof(noise[x]))) {
      for(--x;x>=0;--x) waveOutUnprepareHeader(mysound, &noise[x], sizeof(noise[x]));
      for(x=0;x<7;++x) free(noise[x].lpData); 
      waveOutReset(mysound); waveOutClose(mysound);
      sound=2; mysound=0; return;
    }
  }
}

void snduninit() {
  int x;
  if(!mysound) return;
  waveOutReset(mysound);
  for(x=0;x<7;++x) {
    if(x==0) waveOutUnprepareHeader(mysound, &noise[x], sizeof(noise[x]));
    free(noise[x].lpData);
  }
  waveOutClose(mysound);
  mysound=0;
}

//
//SOUND
//

void playsounds() {
  int x; int y, z, m; char *s;
  if(sound==0&&mysound) { psound=0; snduninit(); }
  if(sound==1&&!mysound) sndinit();
  if(sound!=1) return;
psound=gametime%50?0:3;
/*  for(x=0;x<7;++x) {
    s=noise[x].lpData;
    memset(s, 128, noise[x].dwBufferLength);
//    int sounddef0[]={1200, 1, 0, 1200, 7857727, 255};
    sounddef[1][4]=rnd(5886644);
    for(y=0;y<sounddef[x][1];++y) for(z=0,m=sounddef[x][3+4*y]-sounddef[x][2+4*y];z<m;++z) {
      s[sounddef[x][2+4*y]+z]+=(char)(sounddef[x][5+4*y]*(1-z/(double)m)*sin(sounddef[x][4+4*y]*(3.14159265358979323*2./22050./10000.)*z));
    }
  }*/

  
  for(x=6;x>=0;--x) if(psound&(1<<x)) waveOutWrite(mysound, &noise[x], sizeof(noise[x]));
  for(x=6;x>=0;--x) if(psound&(1<<x)) waveOutWrite(mysound, &noise[x], sizeof(noise[x]));
  psound=0;
}

//
//GRAPHICS
//

unsigned char *tilebit;//[16*16*40*40*4/*tsx*tsy*bpp*/];

void drawtiles() {
  DDSURFACEDESC ds;
  int x, y, a, b, n;
  unsigned short fg, bg;
  if(!(tilebit=(unsigned char *)malloc(16*16*tsx*tsy*bpp))) return;
  fg=colsv[0]; bg=colsv[1]; n=0;
  for(b=0;b<16;++b) for(a=0;a<16;++a) {
    if((a|(b<<4))>colsc[n]) {
      fg=colsv[(++n)<<1]; bg=colsv[(n<<1)|1];
    }
    for(y=b*tsy;y<(b+1)*tsy;++y) for(x=a*tsx;x<(a+1)*tsx;++x)
      ((unsigned short *)tilebit)[x+y*16*tsx]=(bitm[(x>>3)+(16*tsy-1-y)*2*tsx]&(1<<(7^x&7)))?fg:bg;
  }
  //for(y=0;y<16*tsy;++y) for(x=0;x<16*tsx;++x) ((unsigned short *)tilebit)[x+y*16*tsx]=rnd();
  //for(y=0;y<tsy;++y) for(x=0;x<16*tsx;++x) ((unsigned short *)tilebit)[x+y*16*tsx]=31;///*31*/1<<(x/tsx);
  //for(int z=1;z<16;++z) {
  //  if(!(z&1)) for(y=0;y<tsy;++y) ((unsigned short *)tilebit)[tsx-2+z*tsx+y*16*tsx]=65535;
  //  if(!(z&2)) for(y=0;y<tsy;++y) ((unsigned short *)tilebit)[y+z*tsx+(tsy-2)*16*tsx]=65535;
  //  if(!(z&4)) for(y=0;y<tsy;++y) ((unsigned short *)tilebit)[1+z*tsx+y*16*tsx]=65535;
  //  if(!(z&8)) for(y=0;y<tsy;++y) ((unsigned short *)tilebit)[y+z*tsx+16*tsx]=65535;
  //}
  ds.dwSize=sizeof(ds);
  if(dtiles->Lock(0, &ds, DDLOCK_WRITEONLY|DDLOCK_WAIT, 0)) { free(tilebit); return; } //{ huh=1; return; }
  if(!(ds.dwFlags&DDSD_PITCH)) ds.lPitch=16*tsx*bpp;
  for(y=0;y<16*tsy;++y) memcpy(((char *)ds.lpSurface)+y*ds.lPitch, tilebit+y*16*tsx*bpp, 16*tsx*bpp);
  dtiles->Unlock(ds.lpSurface);
  free(tilebit);
}

void drawdisp() {
  RECT r;
  int x, y;
  unsigned char *c=disp;
  for(y=0;y<60;++y) for(x=0;x<80;++x,++c) if(bdisp<0||odisp[bdisp][x+y*80]!=disp[x+y*80]) {
    odisp[bdisp&1][x+y*80]=disp[x+y*80];
    r.left=((*c)&15)*tsx;
    r.top=((*c)>>4)*tsy;
    r.right=r.left+tsx;
    r.bottom=r.top+tsy;
    dback->BltFast(x*tsx, y*tsy, dtiles, &r, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
  }
}

//
//RANDOMITY
//

unsigned int rseeda[624];
int rseedu=624;

void seedr(unsigned int s) {
  rseeda[0]=s;
  for(int n=1;n<624;++n) rseeda[n]=69069u*rseeda[n-1];
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

unsigned int rnd(unsigned int m) {
  unsigned int v, c=(0u-m)/m;
  while((v=rnd())/m>c);
  return(v%m);
}

//
// GAME
//

struct object {
  int x, y2, last;
  int y, next;
  char id, a, b, d;
  int x2;
};

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

int debug=0;

unsigned char sklvtxt[]="          ";

void drawgame() {
  int x, y, r;
  if(debug&2) { if(!keyb[88]) debug^=2; } else if(keyb[88]) debug^=3;
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
    if((!mzlsh)||(r=rnd(516+mzlsh*4))<mzlsh*40) disp[x+(y+4)*80]=maze[(x+mzlx)%mzx+(y+mzly)%mzy*mzx];
    else if(r<520) disp[x+(y+4)*80]=maze[(x+mzlx2)%mzx+(y+mzly2)%mzy*mzx];
    else disp[x+(y+4)*80]=156+(r&3)-((r-520)/12)*4;
  else for(y=0;y<60-4;++y) for(x=0;x<80;++x)
    disp[x+(y+4)*80]=15;
  x=4720;
  if(gamestate&4) { memcpy(disp+x, "Enter new skill level (A-Z)(1-9)     ", 37); x-=80; }
  if((gamestate+1)&~1) {
    memcpy(disp+x, "Play another game? (Y or N)    ", 31);
    if(gamestate&256) if(gamestate&512) disp[x+28]='N'; else disp[x+28]='Y';
    x-=80;
  }
  if((gamestate&3)==1) { memcpy(disp+x, "The SNIPES have triumphed!!!  Long live the SNIPES of the Maze.  ", 65); x-=80; }
  if((gamestate&3)==2) { memcpy(disp+x, "Congratulations --- YOU ACTUALLY WON!!!  ", 41); x-=80; }
}

int lkx=0, lky=0;
void tickgame() {
  if(gamestate==-1) initgame();
  //if(!maze) return;
  lkx=(keyb[57]?2:1)*((keyb[71]||keyb[75]||keyb[79]?-1:0)+(keyb[73]||keyb[77]||keyb[81]?1:0));
  lky=(keyb[57]?2:1)*((keyb[71]||keyb[72]||keyb[73]?-1:0)+(keyb[76]||keyb[79]||keyb[80]||keyb[81]?1:0));

//  lkx=(keyb[57]?2:1)*((keyb[75]?-1:0)+(keyb[77]?1:0));
  mzlx=(mzlx+lkx+mzx)%mzx; //if(rnd(100)==0) lkx=rnd(3)*2-2;
//  lky=(keyb[57]?2:1)*((keyb[72]?-1:0)+(keyb[80]?1:0));
  mzly=(mzly+lky+mzy)%mzy; //if(rnd(100)==0) lky=rnd(3)*2-2;
  mzlsh=0;
  //if(rnd(200)==0) initgame();
  //if(rnd(200)==0) drawtiles();
  if(maze&&objs)
    tickobj();
  drawgame();

  if((!gamestate)&&(keyb[1]||(keyb[29]&&keyb[70]))) gamestate=16;
  if((gamestate+1)&~1) {
    if(!(gamestate&4)) if(keyb[21]) { gamestate|=256; gamestate&=~512; } else if(keyb[49]) gamestate|=768;
    if((!(gamestate&4))&&(gamestate&256)&&keyb[28])
      if(gamestate&512) PostMessage(wind, WM_CLOSE, 0, 0); else gamestate|=12;
    if((gamestate&8)&&!keyb[28]) gamestate&=~8; else if((gamestate&12)==4&&keyb[28]) gamestate=-1;
   // if(keyb[21]) gamestate=-1; else if(keyb[49]) PostMessage(wind, WM_CLOSE, 0, 0);
  }
}

void initgame() {
  int x, y, o;
  if(maze) free(maze); maze=0;
  if(objs) free(objs); objs=0; mobj=0;
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
    do { x=rnd(mzcx); y=rnd(mzcy); }
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
    do { x=rnd(mzcx); y=rnd(mzcy); d=(rnd()&1)+1; }
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
  mzlx=rnd(mzx);
  mzly=rnd(mzy);
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
  x=rnd(mzx); y=rnd(mzy);
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
  int dx, dy, x, y, x2, y2, d;
  int lnumsni;
  if(!objs) return;
  lnumsni=numsni;
  nummot=numsni=numhap=0;
  for(o=objs[0].next;o;o=no) {
    no=objs[o].next;
    switch(objs[o].id) {
      case   0: //Deleteme
                kobj(o); break;
      case   1: //Antisnipe
                if((!objs[o].b)&&(d=checkobj(objs[o].x, objs[o].y, 3, 3)&12)) {
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
                } else if(rnd()&1)
                  { if(!checkobj(objs[o].x, y, 3, 3)) objs[o].y=y; else if(!checkobj(x, objs[o].y, 3, 3)) objs[o].x=x; }
                else
                  { if(!checkobj(x, objs[o].y, 3, 3)) objs[o].x=x; else if(!checkobj(objs[o].x, y, 3, 3)) objs[o].y=y; }
                if(keyb[57]) {
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
                if((!keyb[57])&&(dx||dy)) if(objs[o].a) --objs[o].a; else {
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
                if((!objs[o].b)&&(d=checkobj(objs[o].x, objs[o].y, 2, 2)&4)) {
                  objs[o].b=5;//objs[o].id=0;
                  //drawobj(o, 2, 2, blank);
                  ++nummotd; psound|=16; if(d&4) gamescore+=500; if(gamescore>2000000000) gamescore=2000000000; //--nummot;
                  //break;
                } else if(!objs[o].b) {
                  drawobj(o, 2, 2, (gametime2&1)?pmot1:pmot0);
                  if(lnumsni<maxsni&&!rnd(50)) {
                    d=rnd(12); x=(objs[o].x+mdirxy[d+3]+mzx)%mzx; y=(objs[o].y+mdirxy[d]+mzy)%mzy;
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
                    (objs[o].id!=4?(abulbounce&&(abulbounce==1||!rnd(abulbounce)))
                                  :(sbulbounce&&(sbulbounce==1||!rnd(sbulbounce))))) { objs[o].id=0; break; }
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
                    (objs[o].id!=4?(abulbounce&&(abulbounce==1||!rnd(abulbounce)))
                                  :(sbulbounce&&(sbulbounce==1||!rnd(sbulbounce))))) { objs[o].id=0; break; }
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
                if((!(objs[o].b&1))&&(maze[x+y*mzx]>=160||((objs[o].b&2)&&maze[x2+y2*mzx]>=160))) {
                  objs[o].a=5; objs[o].b|=1;
                  ++numsnid;
                  psound|=8;
                  if((maze[x+y*mzx]>=160&&maze[x+y*mzx]<162)||((objs[o].b&2)&&(maze[x2+y2*mzx]>=160&&maze[x2+y2*mzx]<162)))
                    gamescore+=10;
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
                if((!(objs[o].b&2))&&!objs[o].a) {
                  if(!maze[x2+y2*mzx]) {
                    maze[x2+y2*mzx]=136+d;
                    objs[o].b|=2;
                    objs[o].a=1;
                  } else objs[o].d=rnd()&7;
                  break;
                }
                if((objs[o].b&2)&&!rnd(40)) {
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
                if((objs[o].b&2)&&!objs[o].a) {
                  if(!rnd(7)) d=rnd()&7;
                  dx=(x2+dirxy[d+2]+mzx)%mzx; dy=(y2+dirxy[d]+mzy)%mzy;
                  if((!maze[dx+dy*mzx])||(objs[o].d^d)==4) {
                    maze[ x+ y*mzx]=0;
                    maze[x2+y2*mzx]=128;
                    maze[dx+dy*mzx]=136+d;
                    objs[o].x=x2; objs[o].y=y2;
                    objs[o].a=3; objs[o].d=d;
                  }
                } else --objs[o].a;
                break;
    }
  }
  if(!(gamestate||nummot||numsni)) gamestate=2;
  if(!gamestate) ++gametime; ++gametime2;
}