#include "src/pch.h"
#include "UIController.h"
#include "InputState.h"

namespace energonsoftware {

InputState::MouseState::MouseState()
{
    ZeroMemory(buttons, MouseButton_max * sizeof(bool));
}

InputState::MouseState::~MouseState() throw()
{
}

InputState::GamePadState::GamePadState()
    : left_trigger(0.0f), right_trigger(0.0f)
{
    ZeroMemory(buttons, GamePad_max * sizeof(bool));
}

InputState::GamePadState::~GamePadState() throw()
{
}

void InputState::destroy(InputState* const state, MemoryAllocator* const allocator)
{
    state->~InputState();
    operator delete(state, 16, *allocator);
}

InputState::InputState()
{
    ZeroMemory(_keyboard_state, InputKeySym_max * sizeof(bool));
}

InputState::~InputState() throw()
{
}

void InputState::keyboard_key(InputKeySym key, bool pressed, bool alert)
{
    _keyboard_state[key] = pressed;
    if(alert) {
        if(pressed) {
            UIController::controller()->key_down(key);
        } else {
            UIController::controller()->key_up(key);
        }
    }
}

void InputState::mouse_button(MouseSym button, const Position& position, bool pressed, bool alert)
{
    _mouse_state.position = position;
    _mouse_state.buttons[button] = pressed;
    if(alert) {
        if(pressed) {
            UIController::controller()->mouse_button_down(button, position);
        } else {
            UIController::controller()->mouse_button_up(button, position);
        }
    }
}

}
