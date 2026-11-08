#ifndef SM63_AUDIO_H
#define SM63_AUDIO_H
#include <stdbool.h>
#include <stddef.h>
/* Original sounds through ASND (Wii only). Call audio_init after the course
   renderer has reserved its MEM2; audio_update once per rendered frame plays
   the requests queued by the simulation (sfx.h). */
bool audio_init(void);
void audio_update(bool paused);
void audio_shutdown(void);

/* The story cutscenes (cinema.h): their music decoded as it plays, their
   sound effects decoded into `mem` (32-byte aligned, `size` bytes; returns
   the bytes used, or 0 when they do not fit). audio_cinema_update once per
   rendered frame instead of audio_update while a cutscene plays. */
struct Cinema;
struct CinemaData;
size_t audio_cinema_begin(const struct CinemaData *d, unsigned char *mem, size_t size);
void audio_cinema_update(struct Cinema *p);
void audio_cinema_end(void);
#endif
