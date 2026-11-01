#ifndef SM63_INPUT_H
#define SM63_INPUT_H
#include "game.h"
#ifdef __cplusplus
extern "C" {
#endif
void input_init(void);
GameInput input_poll(void);
void input_shutdown(void);
#ifdef __cplusplus
}
#endif
#endif
