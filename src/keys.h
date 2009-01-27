#ifndef KEYS_H_
#define KEYS_H_

typedef enum
{
    KeySoundOff = 100,
    KeySoundOn,
    KeyResolution,
    KeyFullscreen,
    KeyColours,
    KeyDebug,

    KeySelect,
    KeyUp,
    KeyDown,

    KeyYes,
    KeyNo,

    KeyQuit,
    KeyPause,

    KeyAccel,
    KeyMoveUp,
    KeyMoveDown,
    KeyMoveLeft,
    KeyMoveRight,

    KeyShootUp,
    KeyShootDown,
    KeyShootLeft,
    KeyShootRight,
} Key;

int getKey(Key key);
int checkkey();  // Returns ascii code of a pressed key, or 0 if none.

#endif //KEYS_H_
