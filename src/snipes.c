//GNU GPL-2 or later.
#include <SDL.h>
#include <malloc.h>
#include <stdio.h>
#include <time.h>


///
///ALL
///

#include "snipeSound.h"
#include "random.h"
#include "graphics.h"
#include "colourScheme.h"
#include "skillLevel.h"
#include "settings.h"
#include "keys.h"
#include "help.h"


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
int trace(int *, int *, int, int, int *);
void tickobj();
int snitrace(int, int, int, int, int);
int sniscan(int, int, int, int);
int hapscan(int, int, int, int);

int mzx, mzy, mzcx, mzcy;
int mzlx, mzly, mzlx2, mzly2, mzlsh;
int rmw;

enum
{
    GameUninit = -1,        // Not initialised.
    GameRunning = 0,        // Game is normal.
    GameOverLost = 1,       // Player dead.
    GameOverWon = 2,        // Snipes dead.
    GameSetLevel = 4,       // Want to start new game, choosing level.
    GameWaitLevel = 8,      // Wait before setting level. (Not sure exactly what this does.)
    GameOverAborted = 16,   // Player quit.
    GameAgainYesNo = 256,   // User selected yes or no.
    GameAgainNo = 512,      // User selected no.
};

unsigned char *maze=0;
int gametime=0, gametime2=0, gamescore=0, gamelives=0, gamestate = GameUninit;
int gamePaused = 0;
int gameDisplayHelp = 0;
int nummotd=0, nummot=0, numsnid=0, numsni=0, numhapd=0, numhap=0;

int maxsni, abulbounce, sbulbounce, sniprod, shootrate, targbounce, scanprob, killwall, snikilmot, snikilsni, antkilant, makehappy;

int outofmemory=0;


void cleanupSnipes()
{
    free(maze);
    maze = NULL;
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

int debug=0, chngm=0, chngc=0, chngkd, chngfs=0;

unsigned char sklvtxt[]="          ";
char *soundtxt[3]={"    Sound off    ", "    Sound on     ", "Sound init failed"};

void drawgame() {
  int x, y, r;
  if(debug&2) { if(!getKey(KeyDebug)) debug^=2; } else if(getKey(KeyDebug)) debug^=3;
  if(chngm&2) { if(!getKey(KeyResolution)) chngm^=2; } else if(getKey(KeyResolution)) { chngm^=3; newmode=~getmode(); chngkd=0; }
  if(chngc&2) { if(!getKey(KeyColours)) chngc^=2; } else if(getKey(KeyColours)) { chngc^=3; chngkd=0; }
  if(getKey(KeySoundOff)) enableSound(0); if(getKey(KeySoundOn)) enableSound(1);
  memcpy(disp, " \x10\x11 dead =       | \x80\x88 dead =       | \x00\x81 dead =       | Skill Level  =           "
               " \x12\x13 left =       | \x80\x88 left =       | \x00\x81 left =       | Elapsed Time =           "
               "                                                                                "
               "Men left =                        SNIPES 2D            Game Score   =     000000", 320);
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
  } else memcpy(disp+192, soundtxt[isSoundEnabled()], 17);
  if(outofmemory) memcpy(disp+80*3+33, "OUT OF MEMORY!", 14);
  if(maze) for(y=0;y<60-4;++y) for(x=0;x<80;++x)
    if((!mzlsh)||(r=rndr(516+mzlsh*4))<mzlsh*40) disp[x+(y+4)*80]=maze[(x+mzlx)%mzx+(y+mzly)%mzy*mzx];
    else if(r<520) disp[x+(y+4)*80]=maze[(x+mzlx2)%mzx+(y+mzly2)%mzy*mzx];
    else disp[x+(y+4)*80]=156+(r&3)-((r-520)/12)*4;
  else for(y=0;y<60-4;++y) for(x=0;x<80;++x)
    disp[x+(y+4)*80]=15;
  if(gamePaused && gamestate == GameRunning)
      memcpy(disp+80*24+40-5, "* PAUSED *", 10);
  x=4720;
  if((gamestate & (GameSetLevel | GameWaitLevel)) == GameSetLevel) {
    memcpy(disp+x, "Enter new skill level (A-Z)(0-9)     ", 37);
    if(newskill>=0) disp[x+33]=newskill+'A';
    if(newlevel>=0) disp[x+34]=newlevel+'0';
    x-=80;
  }
  if(gamestate != GameUninit && gamestate != GameRunning) {
    memcpy(disp+x, "Play another game? (Y or N)    ", 31);
    if(gamestate & GameAgainYesNo)
    {
        if(gamestate & GameAgainNo) disp[x+28]='N'; else disp[x+28]='Y';
    }
    x-=80;
  }
  if((gamestate & (GameOverLost|GameOverWon)) == GameOverLost) { memcpy(disp+x, "The SNIPES have triumphed!!!  Long live the SNIPES of the Maze.  ", 65); x-=80; }
  if((gamestate & (GameOverLost|GameOverWon)) == GameOverWon) { memcpy(disp+x, "Congratulations --- YOU ACTUALLY WON!!!  ", 41); x-=80; }
  if(chngm) {
    for(y=1635,x=0;x<8;++x,y+=80) memcpy(disp+y, modes[x].n, 10);
    disp[1635+(~newmode)%100*80]='*';
    if(!chngkd) {
      if(getKey(KeyUp))   { chngkd=1; newmode=~(((~newmode)%100+7)%8+(~newmode)/100*100); }
      if(getKey(KeyDown)) { chngkd=1; newmode=~(((~newmode)%100+1)%8+(~newmode)/100*100); }
    } else if(!(getKey(KeyUp) || getKey(KeyDown))) chngkd=0;
    if(getKey(KeySelect)) { chngm=0; if(getmode()!=~newmode) newmode=~newmode; }
  } else if(!getKey(KeyFullscreen)) chngfs=0; else if(!chngfs) {
    chngfs=1;
    newmode=(getmode()+100)%200;
  }
  if(chngc&&!chngm) {
    for(y=1634,x=0;x<NUMCOLSS;++x,y+=80) memcpy(disp+y, colss[x].n, 12);
    disp[1634+curcols*80]='*';
    if(!chngkd) {
      x=curcols;
      if(getKey(KeyUp))   { chngkd=1; curcols=(curcols+NUMCOLSS-1)%NUMCOLSS; }
      if(getKey(KeyDown)) { chngkd=1; curcols=(curcols+         1)%NUMCOLSS; }
      if(x!=curcols) {
        setColourScheme(&colss[curcols]);
        drawtiles();
      }
    } else if(!(getKey(KeyUp) || getKey(KeyDown))) chngkd=0;
    if(getKey(KeySelect)) { chngc=0; }
  }
}

int lkx=0, lky=0;
void tickgame()
{
 int x;
  if(gamestate == GameUninit) initgame();
  lkx = (getKey(KeyAccel)?2:1)*((getKey(KeyMoveLeft)?-1:0)+(getKey(KeyMoveRight)?1:0));
  lky = (getKey(KeyAccel)?2:1)*((getKey(KeyMoveUp)  ?-1:0)+(getKey(KeyMoveDown) ?1:0));

  if(!getKey(KeyPause))
      gamePaused &= ~2;
  else if(!(gamePaused & 2))
      gamePaused ^= 3;
  if(!getKey(KeyHelp))
      gameDisplayHelp &= ~2;
  else if(!(gameDisplayHelp & 2))
      gameDisplayHelp ^= 3;
  if(initialHelp)
  {
      initialHelp = 0;
      gameDisplayHelp |= 1;
  }

  if((!gamePaused || gamestate != GameRunning) && !gameDisplayHelp)
  {
      mzlx=(mzlx+lkx+mzx)%mzx; //if(rndr(100)==0) lkx=rndr(3)*2-2;
      mzly=(mzly+lky+mzy)%mzy; //if(rndr(100)==0) lky=rndr(3)*2-2;
      mzlsh=0;
      if(maze && objs && !gamePaused)
          tickobj();
  }
  drawgame();
  if(gameDisplayHelp)
      renderHelp(disp, 80, 60);

  if(gamestate == GameRunning && getKey(KeyQuit)) gamestate = GameOverAborted;
  if(gamestate != GameUninit && gamestate != GameRunning) {
    if(!(gamestate & GameSetLevel))
    {
        if(getKey(KeyYes))
        {
            gamestate |= GameAgainYesNo;
            gamestate &= ~GameAgainNo;
        }
        else if(getKey(KeyNo))
            gamestate |= GameAgainYesNo | GameAgainNo;
    }
    if(!(gamestate & GameSetLevel) && (gamestate & GameAgainYesNo) && getKey(KeySelect))
    {
        if(gamestate & GameAgainNo) saidtoquit=1; else gamestate |= GameSetLevel | GameWaitLevel;
    }
    if((gamestate & GameWaitLevel) && !getKey(KeySelect))
    {
        gamestate &= ~GameWaitLevel;
        newskill = newlevel = -1;
    } else if((gamestate & (GameSetLevel | GameWaitLevel)) == GameSetLevel && getKey(KeySelect))
        gamestate = GameUninit;
    if((gamestate & (GameSetLevel | GameWaitLevel)) == GameSetLevel && (x=checkkey()))
    {
        if(x>='0'&&x<='9') newlevel=x-'0'; else if(x>='a'&&x<='z') newskill=x-'a';
    }
  }
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
  gametime=gametime2=gamescore=nummotd=nummot=numsnid=numsni=numhapd=numhap=0;
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
  gamePaused = 0;
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
const int diry[10]={0, 1, 1, 1, 0, -1, -1, -1, 0, 1};
const int *const dirx = diry+2;
const int xydir[9]={5, 6, 7, 4, -1, 0, 3, 2, 1};

//int mdirx[12]={         2, 2, 2, 1, 0, -1, -1, -1, -1, 0, 1, 2};
//int mdiry[12]={0, 1, 2, 2, 2, 2, 1, 0, -1, -1, -1, -1         };
const int  mdirxy[15]={0, 1, 2, 2, 2, 2, 1, 0, -1, -1, -1, -1, 0, 1, 2};

void tickobj() {
  int o, no, co;
  int dx, dy, x, y, x2, y2, d;
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
                    playSound(AntisnipeDeadSound);
                  } else { playSound(AntisnipeDeadForeverSound); if(!gamestate) gamestate = GameOverLost; }
                }
                if(objs[o].b) {
                  if(!(--objs[o].b))
                  {
                      if(objs[o].a)
                      {
                        objs[o].id=0;
                        drawobj(o, 3, 3, blank);
                        break;
                      }
                      else
                      {
                        drawobj(o, 3, 3, blank);
                        objs[o].x=objs[o].x2; objs[o].y=objs[o].y2;
                        mzlx=(objs[o].x+ofx)%mzx;
                        mzly=(objs[o].y+ofy)%mzy;
                        break;
                      }
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
                dx = (getKey(KeyMoveLeft)?-1:0)+(getKey(KeyMoveRight)?1:0);
                dy = (getKey(KeyMoveUp)  ?-1:0)+(getKey(KeyMoveDown) ?1:0);
                x=(objs[o].x+dx+mzx)%mzx;
                y=(objs[o].y+dy+mzy)%mzy;
                if(!checkobj(x, y, 3, 3)) {
                  objs[o].x=x; objs[o].y=y;
                } else if(killwall) goto killedbywall; else if(rnd()&1)
                  { if(!checkobj(objs[o].x, y, 3, 3)) objs[o].y=y; else if(!checkobj(x, objs[o].y, 3, 3)) objs[o].x=x; }
                else
                  { if(!checkobj(x, objs[o].y, 3, 3)) objs[o].x=x; else if(!checkobj(objs[o].x, y, 3, 3)) objs[o].y=y; }
                if(getKey(KeyAccel)) {
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
                dx = (getKey(KeyShootLeft)?-1:0)+(getKey(KeyShootRight)?1:0);
                dy = (getKey(KeyShootUp)  ?-1:0)+(getKey(KeyShootDown) ?1:0);
                if(objs[o].d!=xydir[4+dx+dy*3]) { objs[0].a=0; objs[o].d=xydir[4+dx+dy*3]; }
                if((!getKey(KeyAccel))&&(dx||dy)) if(objs[o].a) --objs[o].a; else {
                  objs[o].a=3;
                  playSound(AntisnipeBulletSound);
                  d=xydir[4+dx+dy*3];
                  x=(objs[o].x+1+dx*2+mzx)%mzx;
                  y=(objs[o].y+1+dy*2+mzy)%mzy;
                  if(maze[x+y*mzx]>=16) maze[x+y*mzx]=160+(rnd()&1); else if((!maze[x+y*mzx])&&(co=nobj(o))) {
                    objs[co].id=3; objs[co].b=rnd()&1; objs[co].d=d;
                    objs[co].x=x; objs[co].y=y;
                    maze[x+y*mzx]=131^objs[co].b;
                  }
                } else objs[o].a=0;
                break;
      case   2: //Mother
                ++nummot;
                if((!objs[o].b)&&(d=checkobj(objs[o].x, objs[o].y, 2, 2)&snikilmot)) {
                  objs[o].b=5;
                  ++nummotd; playSound(MotherDeadSound); if(d&4) gamescore+=500; if(gamescore>2000000000) gamescore=2000000000;
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
                dx=dirx[d]; dy=diry[d];
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
                  else { playSound(BulletBounceSound); co=1; goto abulretr1; }
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
                  else { co=1; playSound(BulletBounceSound); goto abulretr2; }
                }
                maze[x+y*mzx]=objs[o].id==4?136+d:130+objs[o].b;
                objs[o].b^=1;
                objs[o].x=x; objs[o].y=y;
                break;
      case   5: //Snipe
                ++numsni;
                x=objs[o].x; y=objs[o].y;
                d=objs[o].d;
                x2=(x+dirx[d]+mzx)%mzx; y2=(y+diry[d]+mzy)%mzy;
                if((!(objs[o].b&1))&&((maze[x+y*mzx]>=160&&maze[x+y*mzx]<snikilsni)||((objs[o].b&2)&&maze[x2+y2*mzx]>=160&&maze[x2+y2*mzx]<snikilsni))) {
                  objs[o].a=5; objs[o].b|=1;
                  ++numsnid;
                  if((maze[x+y*mzx]>=160&&maze[x+y*mzx]<162)||((objs[o].b&2)&&(maze[x2+y2*mzx]>=160&&maze[x2+y2*mzx]<162)))
                    gamescore+=10;
                  if(makehappy&&!(maze[x+y*mzx]>=160&&maze[x+y*mzx]<snikilsni)) {
                    objs[o].id=6; objs[o].a=1; objs[o].b=0; maze[x+y*mzx]=129; maze[x2+y2*mzx]=0; ++numhap; break;
                  } else playSound(SnipeDeadSound);
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
                if((objs[o].b&2)&&(!rndr(shootrate))&&snitrace(x2, y2, dirx[d], diry[d], targbounce)) {
                  objs[o].a=2;
                  objs[o].b&=~2;
                  x=(x2+dirx[d]+mzx)%mzx;
                  y=(y2+diry[d]+mzy)%mzy;
                  maze[x2+y2*mzx]=0;
                  playSound(SnipeBulletSound);
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
                  dx=(x2+dirx[d]+mzx)%mzx; dy=(y2+diry[d]+mzy)%mzy;
                  while((objs[o].d^d)!=4&&maze[dx+dy*mzx]) {
                    d=rnd()&7;
                    dx=(x2+dirx[d]+mzx)%mzx; dy=(y2+diry[d]+mzy)%mzy;
                  }
                  maze[ x+ y*mzx]=0;
                  maze[x2+y2*mzx]=128;
                  maze[dx+dy*mzx]=136+d;
                  objs[o].x=x2; objs[o].y=y2;
                  objs[o].a=3; objs[o].d=d;
                } else --objs[o].a;
                break;
      case   6: //Happy face
                ++numhap;
                x=objs[o].x; y=objs[o].y;
                if((!(objs[o].b&1))&&maze[x+y*mzx]>=160&&maze[x+y*mzx]<snikilsni) {
                  objs[o].a=5; objs[o].b|=1;
                  ++numhapd;
                  if(maze[x+y*mzx]<162)
                    gamescore+=10;
                  playSound(HappyfaceDeadSound);
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
                  x2=(x+dirx[d]+mzx)%mzx; y2=(y+diry[d]+mzy)%mzy;
                  if(!maze[x2+y2*mzx]) { maze[x2+y2*mzx]=129; maze[x+y*mzx]=0; objs[o].x=x2; objs[o].y=y2; }
                }
                break;
    }
  }
  if(gamestate == GameRunning && nummot == 0 && numsni == 0)
      gamestate = GameOverWon;
  if(gamestate == GameRunning) ++gametime; ++gametime2;
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
    rx=(xp-dirx[ax]+mzx)%mzx; ry=(yp-diry[ax]+mzy)%mzy;
  } while(maze[rx+ry*mzx]!=128);
  if((res|1)==131) return(dsts-dst);
  return((res<32&&res>=20)?~(dsts-dst):0);
}

int sniscan(int xp, int yp, int d, int bounces) {
  int od, a, b, cd;
  char dirs[8]={0,1,2,3,4,5,6,7};
  od=rndr(40320);
  for(a=7;a>=0;--a) {
    cd=dirs[b=od%(a+1)];
    if((cd^d)==4?snitrace((xp+dirx[cd]+mzx)%mzx, (yp+diry[cd]+mzy)%mzy, dirx[cd], diry[cd], bounces)
                :snitrace(xp, yp, dirx[cd], diry[cd], bounces)) return(cd);
    od/=(a+1); dirs[b]=dirs[a];
  }
  return(-1);
}

int hapscan(int xp, int yp, int d, int bounces) {
  int od, a, b, cd, r, s=-1, f=0;
  char dirs[8]={0,1,2,3,4,5,6,7};
  od=rndr(40320);
  for(a=7;a>=0;--a) {
    cd=dirs[b=od%(a+1)];
    r=snitrace(xp, yp, dirx[cd], diry[cd], bounces);
    if(r<~1) return(cd); else if(r==0) s=cd; else if(r>0) f=1;
    od/=(a+1); dirs[b]=dirs[a];
  }
  return(f||(rnd()&1)?(s==-1?(int)rnd()&7:s):d);//s==-1?rnd()&7:s);
}
