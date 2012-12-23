#include "src/pch.h"
#include <SDL/SDL.h>
#include "src/engine/Engine.h"
#include "src/engine/State.h"
#include "src/engine/renderer/Light.h"
#include "src/engine/renderer/Renderer.h"
#include "src/engine/scene/Scene.h"
#include "src/engine/ui/InputState.h"
#include "GameConfiguration.h"
#include "Player.h"
#include "GameUIController.h"

energonsoftware::Logger& GameUIController::logger(energonsoftware::Logger::instance("gled.game.ui.GameUIController"));

void GameUIController::destroy(GameUIController* const controller, energonsoftware::MemoryAllocator* const allocator)
{
    controller->~GameUIController();
    operator delete(controller, 16, *allocator);
}

GameUIController::GameUIController()
    : energonsoftware::UIController()
{
}

GameUIController::~GameUIController() throw()
{
}

void GameUIController::key_down(energonsoftware::InputKeySym key)
{
    GameConfiguration& config(GameConfiguration::instance());

    switch(key)
    {
    case energonsoftware::InputKeySym_b:
        LOG_INFO("Rendering bounds...\n");
        config.render_bounds(!config.render_bounds());
        break;
    case energonsoftware::InputKeySym_f:
        LOG_INFO("Rendering wireframe...\n");
        config.render_wireframe(!config.render_wireframe());
        {
            boost::shared_ptr<energonsoftware::PolygonModeRenderCommand> command(
                boost::dynamic_pointer_cast<energonsoftware::PolygonModeRenderCommand, energonsoftware::RenderCommand>(
                    energonsoftware::RenderCommand::new_render_command(energonsoftware::RenderCommand::RC_POLYGON_MODE)));
            command->face = GL_FRONT_AND_BACK;
            command->mode = config.render_wireframe() ? GL_LINE : GL_FILL;
            energonsoftware::Engine::instance().renderer().command_queue().push(command);
        }
        break;
    case energonsoftware::InputKeySym_h:
        LOG_INFO("Rendering shadows...\n");
        config.render_shadows(!config.render_shadows());
        break;
    case energonsoftware::InputKeySym_k:
        LOG_INFO("Rendering skeletons...\n");
        config.render_skeleton(!config.render_skeleton());
        break;
    case energonsoftware::InputKeySym_l:
        LOG_INFO("Enabling lighting...\n");
        energonsoftware::Light::lighting_enable(!energonsoftware::Light::lighting_enabled());
        break;
    case energonsoftware::InputKeySym_m:
        energonsoftware::Engine::instance().print_memory_details();
        break;
    case energonsoftware::InputKeySym_n:
        LOG_INFO("Rendering normals...\n");
        config.render_normals(!config.render_normals());
        break;
    case energonsoftware::InputKeySym_r:
        config.rotate_actors(!config.rotate_actors());
        break;
    case energonsoftware::InputKeySym_v:
        LOG_INFO("Changing render mode...\n");
        config.render_mode(config.render_mode_bump() ? "vertex" : "bump");
        break;
    case energonsoftware::InputKeySym_f12:
        energonsoftware::Engine::instance().renderer().screenshot("screenshot.png");
        break;
    default:
        break;
    }
}

void GameUIController::key_up(energonsoftware::InputKeySym key)
{
    switch(key)
    {
    case energonsoftware::InputKeySym_escape:
        energonsoftware::Engine::instance().quit();
        break;
    default:
        break;
    }
}

void GameUIController::mouse_button_down(energonsoftware::MouseSym button, const energonsoftware::Position& position)
{
}

void GameUIController::mouse_button_up(energonsoftware::MouseSym button, const energonsoftware::Position& position)
{
}

void GameUIController::update(double elapsed)
{
    const GameConfiguration& config(GameConfiguration::instance());

    const energonsoftware::Size& half_view(energonsoftware::Engine::instance().renderer().half_viewport_size());

    // we have to do this because SDL_WarpMouse pushes an SDL_MOUSEMOTION event
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    const energonsoftware::Position amount(half_view.x() - mx, half_view.y() - my);

    const float mouse_velocity = config.input_sensitivity() * elapsed;
    float p = amount.y() * mouse_velocity;
    /*_pitch += p;

    // 60 degrees in radians
    static const float dr60 = 1.04719755;
std::cout << "p: " << p << "_pitch: " << _pitch << std::endl;
    if(_pitch > dr60) {
        p -= _pitch - dr60;
        _pitch = dr60;
    } else if(_pitch < -dr60) {
        p += _pitch + dr60;
        _pitch = -dr60;
    }*/

    Player::instance()->pitch(p);
    Player::instance()->yaw(amount.x() * mouse_velocity);

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

void GameUIController::grab_input(bool grab)
{
    SDL_WM_GrabInput(grab ? SDL_GRAB_ON : SDL_GRAB_OFF);
}

void GameUIController::show_mouse_cursor(bool show)
{
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

void GameUIController::warp_mouse(const energonsoftware::Position& position)
{
    SDL_WarpMouse(position.x(), position.y());
}
