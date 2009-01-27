#ifndef SKILLLEVEL_H_
#define SKILLLEVEL_H_

typedef struct {
  char const *name;
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
  char const *name;
  //Tiles wide, high, Number of cells wide, high, number of walls missing
  short int mzx, mzy; char mzcx, mzcy; short int rmw;
  //Lives, mothers, max snipes
  char lives, mot; short int sni;
} level;

extern skill skills[26];
extern level levels[10];

extern skill *gameskill;
extern level *gamelevel;
extern int lastskill, lastlevel;

#endif //SKILLLEVEL_H_
