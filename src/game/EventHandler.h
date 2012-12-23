#if !defined __EVENTHANDLER_H__
#define __EVENTHANDLER_H__

#include "src/engine/ui/InputSym.h"

union SDL_Event;

class EventHandler
{
public:
    static EventHandler& instance();

public:
    virtual ~EventHandler() throw();

public:
    bool run();

private:
    energonsoftware::MouseSym mouse_button(int button);
    void handle_event(const SDL_Event& event);
    void handle_key_event(const SDL_Event& event, bool pressed);

private:
    EventHandler();
};

#endif
