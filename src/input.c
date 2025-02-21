#include "input.h"

#include "common.h"
#include "window.h"
#include "camera.h"

static InputHandler s_input_handler;
static bool s_input_handler_initialized = false;

int keyidx_to_keycode(int);
int keycode_to_keyidx(int);
int keyidx_to_char(int);
int modifier_to_modifiercode(KeyModifier);

void input_handler_init(void)
{
    assert(!s_input_handler_initialized);
    assert(window_is_initialized());

    s_input_handler.cursorpos.x = window_get()->size.x;
    s_input_handler.cursorpos.y = window_get()->size.y;

    memset(s_input_handler.key_state,         BUTTONSTATE_UP, sizeof(ButtonState)*N_KEYS);
    memset(s_input_handler.mousebutton_state, BUTTONSTATE_UP, sizeof(ButtonState)*N_MOUSE_BUTTONS);

    s_input_handler_initialized = true;
}

void input_handler_deinit(void)
{
    assert(s_input_handler_initialized);
    s_input_handler_initialized = false;
}

void input_handler_poll(void)
{
    assert(s_input_handler_initialized);

    GLFWwindow *window = window_get()->handle;

    double cursorx, cursory;
    glfwGetCursorPos(window, &cursorx, &cursory);
    s_input_handler.cursorpos.x = (float) cursorx;
    s_input_handler.cursorpos.y = (float) cursory;

    for (int i = 0; i < N_KEYS; ++i) {
        const int keycode = keyidx_to_keycode(i);
        const int newstate = glfwGetKey(window, keycode);
        const int oldstate = s_input_handler.key_state[i];
        if (newstate == GLFW_PRESS) {
            if (oldstate == BUTTONSTATE_UP || oldstate == BUTTONSTATE_JUSTRELEASED) {
                vinfo("key '%c' just pressed", keyidx_to_char(i));
                s_input_handler.key_state[i] = BUTTONSTATE_JUSTPRESSED;
            } else {
                s_input_handler.key_state[i] = BUTTONSTATE_DOWN;
            }
        } else if (newstate == GLFW_RELEASE) {
            if (oldstate == BUTTONSTATE_JUSTPRESSED || oldstate == BUTTONSTATE_DOWN) {
                vinfo("key '%c' just released", keyidx_to_char(i));
                s_input_handler.key_state[i] = BUTTONSTATE_JUSTRELEASED;
            } else {
                s_input_handler.key_state[i] = BUTTONSTATE_UP;
            }
        }
    }

    for (int i = 0; i < N_MODIFIERS; ++i) {
        const int modifiercode = modifier_to_modifiercode(i);
        const int state = glfwGetKey(window, modifiercode);
        if (GLFW_PRESS == state) {
            s_input_handler.keymodifier_state[i] = BUTTONSTATE_DOWN;
        } else {
            s_input_handler.keymodifier_state[i] = BUTTONSTATE_UP;
        }
    }
}

bool key_pressed(int keycode)
{
    assert(s_input_handler_initialized);
    ButtonState state = s_input_handler.key_state[keycode_to_keyidx(keycode)];
    return state == BUTTONSTATE_JUSTPRESSED;
}

bool key_released(int keycode)
{
    assert(s_input_handler_initialized);
    ButtonState state = s_input_handler.key_state[keycode_to_keyidx(keycode)];
    return state == BUTTONSTATE_JUSTRELEASED;
}

bool key_is_down(int keycode)
{
    assert(s_input_handler_initialized);
    ButtonState state = s_input_handler.key_state[keycode_to_keyidx(keycode)];
    return state == BUTTONSTATE_JUSTPRESSED || state == BUTTONSTATE_DOWN;
}

bool key_is_up(int keycode)
{
    assert(s_input_handler_initialized);
    ButtonState state = s_input_handler.key_state[keycode_to_keyidx(keycode)];
    return state == BUTTONSTATE_JUSTRELEASED || state == BUTTONSTATE_UP;
}

bool keymodifier_is_down(KeyModifier modifier)
{
    assert(s_input_handler_initialized);
    ButtonState state = s_input_handler.keymodifier_state[modifier];
    return state == BUTTONSTATE_DOWN;
}

int keyidx_to_keycode(int keyidx)
{
    assert(0 <= keyidx && keyidx < N_KEYS);
    if (keyidx < 10) {
        return GLFW_KEY_0 + keyidx;
    } else {
        return GLFW_KEY_A + (keyidx - 10);
    }
}

int keycode_to_keyidx(int keycode)
{
    assert((GLFW_KEY_0 <= keycode && keycode <= GLFW_KEY_9) 
           || (GLFW_KEY_A <= keycode && keycode <= GLFW_KEY_Z));

    if (GLFW_KEY_0 <= keycode && keycode <= GLFW_KEY_9) {
        return keycode - GLFW_KEY_0;
    } else {
        return (keycode + 10) - GLFW_KEY_A;
    }
}

int keyidx_to_char(int keyidx)
{
    return keyidx_to_keycode(keyidx);
}

int modifier_to_modifiercode(KeyModifier modifier)
{
    switch (modifier) {
    case KEYMODIFIER_L_SHIFT: return GLFW_KEY_LEFT_SHIFT;
    case KEYMODIFIER_L_ALT: return GLFW_KEY_LEFT_ALT;
    case KEYMODIFIER_L_CTRL: return GLFW_KEY_LEFT_CONTROL;
    case KEYMODIFIER_R_SHIFT: return GLFW_KEY_RIGHT_SHIFT;
    case KEYMODIFIER_R_ALT: return GLFW_KEY_RIGHT_ALT;
    case KEYMODIFIER_R_CTRL: return GLFW_KEY_RIGHT_CONTROL;
    }
    __builtin_unreachable();
}
