#include "skillLevel.h"


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
{"P",10,2, 50, 8,1, 475,0,1,1,1,0},//Default
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
{"8", 31*12,31*12,31,31,96,  2, 14, 300},//Default
{"9", 46*12,46*12,46,46,99,  1, 28,4500} //Insane
};

skill *gameskill=&skills[15];
level *gamelevel=&levels[8];
int lastskill=15, lastlevel=8;
