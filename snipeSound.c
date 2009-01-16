#include "snipeSound.h"

#include <math.h>


int psound = 0;
int psoundold[7], psoundnow;
Uint8 sndcopyCache[512];
int sndcopyCacheRemaining = 0;

int sound=1;

//These are the definitions of the sound effects in the game
//Length, number of notes; [start, stop, pitch*10000/Hz, volume]*number of notes
int sounddef0[]={1200, 3, 0, 1200, 7857727, 14, 0, 1200, 23573180, 5, 0, 1200, 39288633, 3};
int sounddef1[]={1200, 3, 0, 1200, 5886644, 19, 0, 1200, 17659931, 6, 0, 1200, 29433219, 4};
int sounddef2[]={1400, 6, 0, 700, 8820000, 13, 700, 1400, 10488806, 11,
                          0, 700, 26460000, 4, 700, 1400, 31466420, 4,
                          0, 700, 44100000, 3, 700, 1400, 52444034, 2};
int sounddef3[]={2000, 9, 0, 800, 1102500, 100, 600, 1400, 1651883, 67, 1200, 2000, 2205000, 50,
                          0, 800, 3307500, 33, 600, 1400, 4955651, 22, 1200, 2000, 6615000, 17,
                          0, 800, 5512500, 20, 600, 1400, 8259418, 13, 1200, 2000, 11025000, 10};
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
//The sound effects are for, in this order:
//Snipe-bullet, Antisnipe-bullet, Bullet-bounce, Snipe-dead, Mot-dead, Antisnipe-dead-forever, Antisnipe-dead, Happy-dead

int mysound=0;
char *soundwvs[8];
char *soundring;
int soundringpos;


void enableSound(int state)
{
    sound = state;
}

int isSoundEnabled()
{
    return sound;
}

void playSound(SoundType sound)
{
    psound |= 1<<sound;
}

//Copy 512 bytes of sound, from every sound clip that should be playing
//Each sound may start once at every 512/22050Hz ~ 23ms boundary.
//len must be exactly 512.
static void sndcopy_orig(void *nothing, Uint8 *snd, int len) {
    (void)nothing;
  int x, y, z, m, d;
  int nb=sizeof(psoundold)/sizeof(int);
  int unclipped[512];
  psoundold[psoundnow]=psound; psound=0;
  if(len!=512) { printf("Grrr, got passed a buffer that wasn't exactly 512 bytes... (Was %d)\n", len); return; } //Just in case...
  for(x=0;x<len;++x) unclipped[x]=0;
  for(z=0;z<8;++z) for(y=0;y<nb;++y) if(psoundold[y]&1<<z) {
    d=(psoundnow-y+nb)%nb*512;
    m=sounddef[z][0]-d; if(m>512) m=512;
    for(x=0;x<m;++x) unclipped[x]+=soundwvs[z][x+d];
  }
  for(x=0;x<len;++x) ((char *)snd)[x]=unclipped[x]<=127?unclipped[x]>=-128?unclipped[x]:-128:127;
  psoundnow=(psoundnow+1)%nb;
}

//Copy up to 512 bytes of sound, from every sound clip that should be playing
//Each sound may start once at every 512/22050Hz ~ 23ms boundary.
void sndcopy(void *nothing, Uint8 *snd, int len)
{
    int copy = len < sndcopyCacheRemaining? len : sndcopyCacheRemaining;
    memcpy(snd, sndcopyCache + 512 - sndcopyCacheRemaining, copy);
    sndcopyCacheRemaining -= copy;

    if(len == copy)
        return;  // Finished copying.

    // Fill cache.
    sndcopy_orig(nothing, sndcopyCache, 512);  // Hack, len must be exactly 512, but SDL isn't providing that any more.
    sndcopyCacheRemaining = 512;

    sndcopy(nothing, snd + copy, len - copy);  // Tail call, will return next time.
}

void sndinit() {
; SDL_AudioSpec rq
; if(mysound) {
;   snduninit()
; }
  int x;
  for(psoundnow=x=0; (unsigned)x<sizeof(psoundold)/sizeof(int); ++x) psoundold[x]=0;
  int y, z, m;
  char *s;
  for(x=0;x<8;++x) {
    if(!(soundwvs[x]=(char *)malloc(sounddef[x][0]))) {
      for(--x;x>=0;--x) free(soundwvs[x]); 
        sound=2; return;
    }
  }
  //Generate sound waves from sound definitions, ready to be played
  for(x=0;x<8;++x) {
    s=soundwvs[x];
    memset(s, 0, sounddef[x][0]);
    for(y=0;y<sounddef[x][1];++y) for(z=0,m=sounddef[x][3+4*y]-sounddef[x][2+4*y];z<m;++z) {
      s[sounddef[x][2+4*y]+z]+=(char)(sounddef[x][5+4*y]*(1-z/(double)m)*sin(sounddef[x][4+4*y]*(3.14159265358979323*2./22050./10000.)*z));
    }
  }
; rq.freq=22050
; rq.format=AUDIO_S8
; rq.channels=1
; rq.samples=512
; rq.callback=sndcopy
; rq.userdata=0
; if(0>SDL_OpenAudio(&rq, 0)) {
;   for(x=0;x<8;++x) free(soundwvs[x]);
;   sound=2;
;   return
; }
; SDL_PauseAudio(0)
; psound=0 //Don't play all sounds that would have been played while sound was off
; mysound=1
;
}

void snduninit() {
; int x
; if(mysound) {
;   SDL_CloseAudio()
;   for(x=0;x<8;++x) free(soundwvs[x])
;   mysound=0
; }
}

//
//SOUND
//

void playsounds() {
  if(sound==0&&mysound) { psound=0; snduninit(); }
  if(sound==1&&!mysound) sndinit();
}
