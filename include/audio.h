#ifndef SM63_AUDIO_H
#define SM63_AUDIO_H
#include <stdbool.h>
/* Original sounds through ASND (Wii only). Call audio_init after the course
   renderer has reserved its MEM2; audio_update once per rendered frame plays
   the requests queued by the simulation (sfx.h). */
bool audio_init(void);
void audio_update(bool paused);
void audio_shutdown(void);
#endif
