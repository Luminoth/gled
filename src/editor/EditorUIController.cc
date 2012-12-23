#include "src/pch.h"
#include "src/engine/Engine.h"
#include "src/engine/State.h"
#include "src/engine/renderer/Renderer.h"
#include "src/engine/scene/Scene.h"
#include "src/engine/ui/InputState.h"
#include "src/game/Player.h"
#include "EditorConfiguration.h"
#include "EditorUIController.h"

energonsoftware::Logger& EditorUIController::logger(energonsoftware::Logger::instance("gled.editor.EditorUIController"));

void EditorUIController::destroy(EditorUIController* const controller, energonsoftware::MemoryAllocator* const allocator)
{
    controller->~EditorUIController();
    operator delete(controller, 16, *allocator);
}

EditorUIController::EditorUIController()
    : energonsoftware::UIController(), _mouse_move(false)
{
}

EditorUIController::~EditorUIController() throw()
{
}

void EditorUIController::key_down(energonsoftware::InputKeySym key)
{
    switch(key)
    {
    case energonsoftware::InputKeySym_f12:
        energonsoftware::Engine::instance().renderer().screenshot("screenshot.png");
        break;
    default:
        break;
    }
}

void EditorUIController::key_up(energonsoftware::InputKeySym key)
{
}

void EditorUIController::mouse_button_down(energonsoftware::MouseSym button, const energonsoftware::Position& position)
{
    if(energonsoftware::MouseButton_right == button) {
        _mouse_move = true;
        _last_mouse_position = position;
    }
}

void EditorUIController::mouse_button_up(energonsoftware::MouseSym button, const energonsoftware::Position& position)
{
    if(energonsoftware::MouseButton_right == button) {
        _mouse_move = false;
        _last_mouse_position = position;
    }
}

void EditorUIController::update(double elapsed)
{
    const EditorConfiguration& config(EditorConfiguration::instance());

    if(_mouse_move) {
        const energonsoftware::Position& current_pos(energonsoftware::Engine::instance().input_state().mouse_position());
        const energonsoftware::Position amount(current_pos - _last_mouse_position);

        const float mouse_velocity = config.input_sensitivity()/* * elapsed*/;

        Player::instance()->pitch(amount.y() * mouse_velocity);
        Player::instance()->yaw(amount.x() * mouse_velocity);

        _last_mouse_position = current_pos;
    }

    energonsoftware::Vector3 v;
    if(energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_w)) {
        v.z(v.z() - Player::MOVE_SPEED);
    }

    if(energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_s)) {
        v.z(v.z() + Player::MOVE_SPEED);
    }

    if(energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_a)) {
        v.x(v.x() - Player::MOVE_SPEED);
    }

    if(energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_d)) {
        v.x(v.x() + Player::MOVE_SPEED);
    }

    // rotate the velocity
    v = Player::instance()->orientation() * v;

    // constrain movement along the y-axis
    v.y(0.0f);

    if(energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_space)) {
        v.y(v.y() + Player::MOVE_SPEED);
    }

    if(energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_lshift)) {
        v.y(v.y() - Player::MOVE_SPEED);
    }

    Player::instance()->velocity(v);
}

void EditorUIController::grab_input(bool grab)
{
}

void EditorUIController::show_mouse_cursor(bool show)
{
}

void EditorUIController::warp_mouse(const energonsoftware::Position& position)
{
}
