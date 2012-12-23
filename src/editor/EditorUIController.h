#if !defined __EDITORUICONTROLLER_H__
#define __EDITORUICONTROLLER_H__

#include "src/engine/ui/UIController.h"

class EditorUIController : public energonsoftware::UIController
{
public:
    static void destroy(EditorUIController* const controller, energonsoftware::MemoryAllocator* const allocator);

private:
    static energonsoftware::Logger& logger;

public:
    EditorUIController();
    virtual ~EditorUIController() throw();

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
    energonsoftware::Position _last_mouse_position;
    bool _mouse_move;

private:
    DISALLOW_COPY_AND_ASSIGN(EditorUIController);
};

#endif
