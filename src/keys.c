#include "keys.h"
#include "graphics.h"

#include <assert.h>


int getKey(Key key)
{
    switch(key)
    {
        case KeySoundOff:   return keyb[59];  // F1
        case KeySoundOn:    return keyb[60];  // F2
        case KeyResolution: return keyb[66];  // F8
        case KeyFullscreen: return (
                                    keyb[56]| // Alt
                                    keyb[105] // Right Alt
                                   )&
                                   (
                                    keyb[28]| // Enter
                                    keyb[100] // Keypad Enter
                                   );
        case KeyColours:    return keyb[67];  // F9
        case KeyDebug:      return keyb[88];  // F12


        case KeySelect:     return keyb[28]|  // Enter
                                   keyb[57]|  // Space
                                   keyb[100]; // Keypad Enter
        case KeyUp:         return keyb[72]|  // Keypad Up
                                   keyb[90];  // Up
        case KeyDown:       return keyb[76]|  // Keypad Mid
                                   keyb[80]|  // Keypad Down
                                   keyb[96];  // Down


        case KeyYes:        return keyb[21];  // y
        case KeyNo:         return keyb[49];  // n


        case KeyQuit:       return keyb[1]|   // Escape
                                   (
                                    (
                                     keyb[29]|// Control
                                     keyb[101]// Right Control
                                    )&
                                    (
                                     keyb[70]|// Scrolllock
                                     keyb[102]// Pause/Break
                                    )
                                   );
        case KeyPause:      return keyb[25]|  // p
                                   (
                                    keyb[102]&// Pause/Break
                                   ~(
                                     keyb[29]|// Control
                                     keyb[101]// Right Control
                                    )
                                   );


        case KeyAccel:      return keyb[15]|  // Tab
                                   keyb[28]|  // Enter
                                   keyb[29]|  // Ctrl
                                   keyb[41]|  // `
                                   keyb[42]|  // Shift
                                   keyb[47]|  // v
                                   keyb[48]|  // b
                                   keyb[49]|  // n
                                   keyb[50]|  // m
                                   keyb[51]|  // ,
                                   keyb[52]|  // .
                                   keyb[53]|  // /
                                   keyb[54]|  // Right Shift
                                   keyb[57]|  // Space
                                   keyb[58]|  // Capslock
                                   keyb[74]|  // Keypad -
                                   keyb[78]|  // Keypad +
                                   keyb[82]|  // Keypad Insert
                                   keyb[83]|  // Keypad Delete
                                   keyb[100]; // Keypad Enter
        case KeyMoveUp:     return keyb[71]|  // Keypad Home
                                   keyb[72]|  // Keypad Up
                                   keyb[73]|  // Keypad PgUp
                                   keyb[90];  // Up
        case KeyMoveDown:   return keyb[76]|  // Keypad Mid
                                   keyb[79]|  // Keypad End
                                   keyb[80]|  // Keypad Down
                                   keyb[81]|  // Keypad PgDn
                                   keyb[96];  // Down
        case KeyMoveLeft:   return keyb[71]|  // Keypad Home
                                   keyb[75]|  // Keypad Left
                                   keyb[79]|  // Keypad End
                                   keyb[92];  // Left
        case KeyMoveRight:  return keyb[73]|  // Keypad PgUp
                                   keyb[77]|  // Keypad Right
                                   keyb[81]|  // Keypad PgDn
                                   keyb[94];  // Right


        case KeyShootUp:    return keyb[16]|  // q
                                   keyb[17]|  // w
                                   keyb[18];  // e
        case KeyShootDown:  return keyb[31]|  // s
                                   keyb[44]|  // z
                                   keyb[45]|  // x
                                   keyb[46];  // c
        case KeyShootLeft:  return keyb[16]|  // q
                                   keyb[30]|  // a
                                   keyb[44];  // z
        case KeyShootRight: return keyb[18]|  // e
                                   keyb[32]|  // d
                                   keyb[46];  // c
    };

    assert(0);
}

static char const keycodes[]="1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./";

int checkkey() {
    int x;
    for(x= 2;x<=13;++x) if(keyb[x]) return(keycodes[x-2]);
    for(x=16;x<=27;++x) if(keyb[x]) return(keycodes[x-4]);
    for(x=30;x<=40;++x) if(keyb[x]) return(keycodes[x-6]);
    for(x=44;x<=53;++x) if(keyb[x]) return(keycodes[x-9]);
    return(0);
}
