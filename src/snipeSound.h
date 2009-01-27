#ifndef SNIPE_SOUND_H
#define SNIPE_SOUND_H

#include <SDL.h>

typedef enum
{
    SnipeBulletSound,
    AntisnipeBulletSound,
    BulletBounceSound,
    SnipeDeadSound,
    MotherDeadSound,
    AntisnipeDeadForeverSound,
    AntisnipeDeadSound,
    HappyfaceDeadSound,
} SoundType;

void enableSound(int state);  // 1 = enable, 0 = disable.
int isSoundEnabled();
void playSound(SoundType sound);
void sndcopy(void *, Uint8 *, int);
void sndinit();
void snduninit();
void playsounds();

#endif //SNIPE_SOUND_H
