#ifndef INPUT_H
#define INPUT_H

#include "common.h"

#define N_KEYS          (36)
#define N_MOUSE_BUTTONS (7)
#define N_MODIFIERS     (6)

typedef enum ButtonState {
    BUTTONSTATE_UP = 0,
    BUTTONSTATE_JUSTPRESSED,
    BUTTONSTATE_DOWN,
    BUTTONSTATE_JUSTRELEASED
} ButtonState;

typedef enum KeyModifier {
    KEYMODIFIER_L_SHIFT = 0,
    KEYMODIFIER_L_CTRL,
    KEYMODIFIER_L_ALT,
    KEYMODIFIER_R_SHIFT,
    KEYMODIFIER_R_CTRL,
    KEYMODIFIER_R_ALT,
} KeyModifier;

typedef struct InputHandler {
    vec2s       cursorpos;
    ButtonState key_state[N_KEYS];
    ButtonState mousebutton_state[N_MOUSE_BUTTONS];
    ButtonState keymodifier_state[N_MODIFIERS];
} InputHandler;

void input_handler_init(void);
void input_handler_deinit(void);
void input_handler_poll();

bool key_pressed(int keycode);
bool key_released(int keycode);
bool key_is_down(int keycode);
bool key_is_up(int keycode);

bool keymodifier_is_down(KeyModifier modifier);

#endif
