#include "src/pch.h"
#include <wx/wx.h>
#include "src/engine/Engine.h"
#include "src/engine/renderer/Renderer.h"
#include "src/engine/ui/InputState.h"
#include "src/editor/Application.h"
#include "src/editor/EditorConfiguration.h"
#include "Canvas.h"

// http://wiki.wxwidgets.org/WxGLCanvas

BEGIN_EVENT_TABLE(Canvas, wxGLCanvas)
    EVT_ERASE_BACKGROUND(Canvas::OnEraseBackground)
    EVT_PAINT(Canvas::OnPaint)
    EVT_SIZE(Canvas::OnSize)
    EVT_LEFT_DOWN(Canvas::OnMouseLeftDown)
    EVT_LEFT_UP(Canvas::OnMouseLeftUp)
    EVT_LEFT_DCLICK(Canvas::OnMouseLeftDoubleClick)
    EVT_MIDDLE_DOWN(Canvas::OnMouseMiddleDown)
    EVT_MIDDLE_UP(Canvas::OnMouseMiddleUp)
    EVT_MIDDLE_DCLICK(Canvas::OnMouseMiddleDoubleClick)
    EVT_RIGHT_DOWN(Canvas::OnMouseRightDown)
    EVT_RIGHT_UP(Canvas::OnMouseRightUp)
    EVT_RIGHT_DCLICK(Canvas::OnMouseRightDoubleClick)
    EVT_MOTION(Canvas::OnMouseMotion)
    EVT_ENTER_WINDOW(Canvas::OnMouseEnterWindow)
    EVT_LEAVE_WINDOW(Canvas::OnMouseLeaveWindow)
    EVT_MOUSEWHEEL(Canvas::OnMouseWheel)
    EVT_KEY_DOWN(Canvas::OnKeyDown)
    EVT_KEY_UP(Canvas::OnKeyUp)
END_EVENT_TABLE()

int Canvas::ATTRIB_LIST[] = {
    WX_GL_RGBA, WX_GL_DOUBLEBUFFER,
    WX_GL_BUFFER_SIZE, 32,

    // 2xFSAA (requires GL_ARB_multisample)
    /*WX_GL_SAMPLE_BUFFERS, GL_TRUE,
    WX_GL_SAMPLES, 2,*/

    WX_GL_DEPTH_SIZE, 24,
    WX_GL_STENCIL_SIZE, 8,
    0
};

energonsoftware::Logger& Canvas::logger(energonsoftware::Logger::instance("gled.editor.gui.Canvas"));

Canvas::Canvas(wxWindow* parent, int attribs[])
    : wxGLCanvas(parent, wxID_ANY, attribs),
        _record_mouse(false)
{
    _context = new wxGLContext(this);

    // prevent flashing in windows
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

Canvas::~Canvas() throw()
{
    delete _context;
}

void Canvas::OnEraseBackground(wxEraseEvent& event)
{
    // intentionally empty to fix screen flicker under windows
}

void Canvas::OnPaint(wxPaintEvent& event)
{
    SetCurrent(*_context);

    if(reinterpret_cast<wxTopLevelWindow*>(wxGetApp().GetTopWindow())->IsActive()) {
        wxPaintDC(this);

        if(energonsoftware::Engine::instance().ready()) {
            energonsoftware::Engine::instance().start_frame();
        }

        SwapBuffers();
    }

    event.Skip();
}

void Canvas::OnSize(wxSizeEvent& event)
{
    const EditorConfiguration& config(EditorConfiguration::instance());
    if(energonsoftware::Engine::instance().ready()) {
        const wxSize& size(event.GetSize());
        energonsoftware::Engine::instance().renderer().resize_viewport(size.GetWidth(), size.GetHeight(), config.game_fov());
    }
    Update();

    event.Skip();
}

void Canvas::OnMouseLeftDown(wxMouseEvent& event)
{
    if(_record_mouse) {
        energonsoftware::Engine::instance().input_state().mouse_button(energonsoftware::MouseButton_left,
            energonsoftware::Position(event.GetX(), event.GetY()), true);
    }

    event.Skip();
}

void Canvas::OnMouseLeftUp(wxMouseEvent& event)
{
    if(_record_mouse) {
        energonsoftware::Engine::instance().input_state().mouse_button(energonsoftware::MouseButton_left,
            energonsoftware::Position(event.GetX(), event.GetY()), false);
    }

    event.Skip();
}

void Canvas::OnMouseLeftDoubleClick(wxMouseEvent& event)
{
    if(_record_mouse) {
        /*energonsoftware::Engine::instance().input_state().mouse_button_double(energonsoftware::MouseButton_left,
            energonsoftware::Position(event.GetX(), event.GetY()));*/
    }

    event.Skip();
}

void Canvas::OnMouseMiddleDown(wxMouseEvent& event)
{
    if(_record_mouse) {
        energonsoftware::Engine::instance().input_state().mouse_button(energonsoftware::MouseButton_middle,
            energonsoftware::Position(event.GetX(), event.GetY()), true);
    }

    event.Skip();
}

void Canvas::OnMouseMiddleUp(wxMouseEvent& event)
{
    if(_record_mouse) {
        energonsoftware::Engine::instance().input_state().mouse_button(energonsoftware::MouseButton_middle,
            energonsoftware::Position(event.GetX(), event.GetY()), false);
    }

    event.Skip();
}

void Canvas::OnMouseMiddleDoubleClick(wxMouseEvent& event)
{
    if(_record_mouse) {
        /*energonsoftware::Engine::instance().input_state().mouse_button_double(energonsoftware::MouseButton_middle,
            energonsoftware::Position(event.GetX(), event.GetY()));*/
    }

    event.Skip();
}

void Canvas::OnMouseRightDown(wxMouseEvent& event)
{
    if(_record_mouse) {
        energonsoftware::Engine::instance().input_state().mouse_button(energonsoftware::MouseButton_right,
            energonsoftware::Position(event.GetX(), event.GetY()), true);
    }

    event.Skip();
}

void Canvas::OnMouseRightUp(wxMouseEvent& event)
{
    if(_record_mouse) {
        energonsoftware::Engine::instance().input_state().mouse_button(energonsoftware::MouseButton_right,
            energonsoftware::Position(event.GetX(), event.GetY()), false);
    }

    event.Skip();
}

void Canvas::OnMouseRightDoubleClick(wxMouseEvent& event)
{
    if(_record_mouse) {
        /*energonsoftware::Engine::instance().input_state().mouse_button_double(energonsoftware::MouseButton_right,
            energonsoftware::Position(event.GetX(), event.GetY()));*/
    }

    event.Skip();
}

void Canvas::OnMouseMotion(wxMouseEvent& event)
{
    if(_record_mouse) {
        energonsoftware::Engine::instance().input_state().mouse_position(energonsoftware::Position(event.GetX(), event.GetY()));
    }

    event.Skip();
}

void Canvas::OnMouseEnterWindow(wxMouseEvent& event)
{
    _record_mouse = true;
    event.Skip();
}

void Canvas::OnMouseLeaveWindow(wxMouseEvent& event)
{
    _record_mouse = false;
    event.Skip();
}

void Canvas::OnMouseWheel(wxMouseEvent& event)
{
    if(_record_mouse) {
        // TODO: handle this?
    }

    event.Skip();
}

void Canvas::OnKeyDown(wxKeyEvent& event)
{
    handle_key_event(event, true);
    event.Skip();
}

void Canvas::OnKeyUp(wxKeyEvent& event)
{
    handle_key_event(event, false);
    event.Skip();
}

void Canvas::handle_key_event(const wxKeyEvent& event, bool pressed)
{
    // TODO: this list is not complete
    switch(event.GetKeyCode())
    {
    case WXK_BACK:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_backspace, pressed);
        break;
    case WXK_TAB:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_tab, pressed);
        break;
    case WXK_RETURN:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_return, pressed);
        break;
    case WXK_ESCAPE:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_escape, pressed);
        break;
    case WXK_SPACE:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_space, pressed);
        break;
    case WXK_SHIFT:
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_lshift, pressed);
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_rshift, pressed);
        break;
    case 'a':
    case 'A':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_a, pressed);
        break;
    case 'b':
    case 'B':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_b, pressed);
        break;
    case 'c':
    case 'C':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_c, pressed);
        break;
    case 'd':
    case 'D':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_d, pressed);
        break;
    case 'e':
    case 'E':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_e, pressed);
        break;
    case 'f':
    case 'F':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_f, pressed);
        break;
    case 'g':
    case 'G':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_g, pressed);
        break;
    case 'h':
    case 'H':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_h, pressed);
        break;
    case 'i':
    case 'I':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_i, pressed);
        break;
    case 'j':
    case 'J':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_j, pressed);
        break;
    case 'k':
    case 'K':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_k, pressed);
        break;
    case 'l':
    case 'L':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_l, pressed);
        break;
    case 'm':
    case 'M':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_m, pressed);
        break;
    case 'n':
    case 'N':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_n, pressed);
        break;
    case 'o':
    case 'O':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_o, pressed);
        break;
    case 'p':
    case 'P':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_p, pressed);
        break;
    case 'q':
    case 'Q':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_q, pressed);
        break;
    case 'r':
    case 'R':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_r, pressed);
        break;
    case 's':
    case 'S':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_s, pressed);
        break;
    case 't':
    case 'T':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_t, pressed);
        break;
    case 'u':
    case 'U':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_u, pressed);
        break;
    case 'v':
    case 'V':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_v, pressed);
        break;
    case 'w':
    case 'W':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_w, pressed);
        break;
    case 'x':
    case 'X':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_x, pressed);
        break;
    case 'y':
    case 'Y':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_y, pressed);
        break;
    case 'z':
    case 'Z':
        energonsoftware::Engine::instance().input_state().keyboard_key(energonsoftware::InputKeySym_z, pressed);
        break;
    }
}
