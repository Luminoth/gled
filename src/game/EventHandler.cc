#include "src/pch.h"
#include <SDL/SDL.h>
#include "src/engine/Engine.h"
#include "src/engine/ui/InputState.h"
#include "EventHandler.h"

EventHandler& EventHandler::instance()
{
    static boost::shared_ptr<EventHandler> event_handler;
    if(!event_handler) {
        // NOTE: this doesn't go on an allocator
        event_handler.reset(new EventHandler());
    }
    return *event_handler;
}

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler() throw()
{
}

bool EventHandler::run()
{
    // NOTE: can't thread this off the video thread. booo :(
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        handle_event(event);
        switch(event.type)
        {
        case SDL_ACTIVEEVENT:
            // TODO: handle this
            break;
        case SDL_QUIT:
            return false;
        }
    }
    return true;
}

energonsoftware::MouseSym EventHandler::mouse_button(int button)
{
    switch(button)
    {
    case SDL_BUTTON_LEFT:
        return energonsoftware::MouseButton_left;
    case SDL_BUTTON_MIDDLE:
        return energonsoftware::MouseButton_middle;
    case SDL_BUTTON_RIGHT:
        return energonsoftware::MouseButton_right;
    }
    return energonsoftware::MouseButton_left;
}

void EventHandler::handle_event(const SDL_Event& event)
{
    // NOTE: we don't handle SDL_MOUSEMOTION here because SDL_WarpMouse pushes it out

    switch(event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if(event.button.button == SDL_BUTTON_WHEELDOWN) {
            //_mouse_state.wheel_y -= 1.0f;
        } else {
            energonsoftware::Engine::instance().input_state().mouse_button(mouse_button(event.button.button), energonsoftware::Position(event.button.x, event.button.y), true);
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if(event.button.button == SDL_BUTTON_WHEELUP) {
            //_mouse_state.wheel_y += 1.0f;
        } else {
            energonsoftware::Engine::instance().input_state().mouse_button(mouse_button(event.button.button), energonsoftware::Position(event.button.x, event.button.y), false);
        }
        break;
    /*case SDL_MOUSEWHEEL:
        // TODO: handle this?
        break;*/
    case SDL_KEYDOWN:
        handle_key_event(event, true);
        break;
    case SDL_KEYUP:
        handle_key_event(event, false);
        break;
    default:
        break;
    }
}

void EventHandler::handle_key_event(const SDL_Event& event, bool pressed)
{
    // TODO: this list is not complete
    switch(event.key.keysym.sym)
    {
    case SDLK_BACKSPACE:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_backspace, pressed);
        break;
    case SDLK_TAB:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_tab, pressed);
        break;
    case SDLK_RETURN:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_return, pressed);
        break;
    case SDLK_PAUSE:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_pause, pressed);
        break;
    case SDLK_ESCAPE:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_escape, pressed);
        break;
    case SDLK_SPACE:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_space, pressed);
        break;
    case SDLK_RSHIFT:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_rshift, pressed);
        break;
    case SDLK_LSHIFT:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_lshift, pressed);
        break;
    case SDLK_a:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_a, pressed);
        break;
    case SDLK_b:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_b, pressed);
        break;
    case SDLK_c:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_c, pressed);
        break;
    case SDLK_d:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_d, pressed);
        break;
    case SDLK_e:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_e, pressed);
        break;
    case SDLK_f:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_f, pressed);
        break;
    case SDLK_g:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_g, pressed);
        break;
    case SDLK_h:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_h, pressed);
        break;
    case SDLK_i:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_i, pressed);
        break;
    case SDLK_j:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_j, pressed);
        break;
    case SDLK_k:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_k, pressed);
        break;
    case SDLK_l:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_l, pressed);
        break;
    case SDLK_m:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_m, pressed);
        break;
    case SDLK_n:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_n, pressed);
        break;
    case SDLK_o:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_o, pressed);
        break;
    case SDLK_p:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_p, pressed);
        break;
    case SDLK_q:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_q, pressed);
        break;
    case SDLK_r:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_r, pressed);
        break;
    case SDLK_s:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_s, pressed);
        break;
    case SDLK_t:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_t, pressed);
        break;
    case SDLK_u:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_u, pressed);
        break;
    case SDLK_v:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_v, pressed);
        break;
    case SDLK_w:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_w, pressed);
        break;
    case SDLK_x:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_x, pressed);
        break;
    case SDLK_y:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_y, pressed);
        break;
    case SDLK_z:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_z, pressed);
        break;
    default:
        break;
    }
}
