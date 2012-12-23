#if !defined __GAMEUICONTROLLER_H__
#define __GAMEUICONTROLLER_H__

#include "src/engine/ui/UIController.h"

class GameUIController : public energonsoftware::UIController
{
public:
    static void destroy(GameUIController* const controller, energonsoftware::MemoryAllocator* const allocator);

private:
    static energonsoftware::Logger& logger;

public:
    GameUIController();
    virtual ~GameUIController() throw();

public:
    virtual void key_down(energonsoftware::InputKeySym key);
    virtual void key_up(energonsoftware::InputKeySym key);
    virtual void mouse_button_down(energonsoftware::MouseSym button, const energonsoftware::Position& position);
    virtual void mouse_button_up(energonsoftware::MouseSym button, const energonsoftware::Position& position);

    virtual void update(double elapsed);

    virtual void grab_input(bool grab);
    virtual void show_mouse_cursor(bool show);
    virtual void warp_mouse(const energonsoftware::Position& position);

private:
    DISALLOW_COPY_AND_ASSIGN(GameUIController);
};

#endif
