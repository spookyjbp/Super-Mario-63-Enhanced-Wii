#ifndef SM63_INPUT_H
#define SM63_INPUT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float horizontal;
    bool jump_pressed;
    bool exit_pressed;
} InputState;

void input_init(void);
InputState input_poll(void);
void input_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
