#ifndef SETTINGS_H_
#define SETTINGS_H_

extern int curcols;  // Current colour scheme.
extern int saidtoquit;  // Should quit main loop.
extern int lcurd[18];  // TODO: Remember/figure out what this variable was for.
extern int newmode;  // If this is set, change display mode/resolution on next tick.
extern int initialHelp;

static int const dispWidth = 80;
static int const dispHeight = 60;
extern unsigned char disp[80*60];  // The screen buffer. Doesn't belong here, but putting here for now...

#endif //SETTINGS_H_
