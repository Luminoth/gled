#if !defined __MAINFRAME_H__
#define __MAINFRAME_H__

#include <wx/frame.h>

class Canvas;
class ConsolePanel;
class wxMouseCaptureLostEvent;
class wxMoveEvent;
class wxSizeEvent;

class MainFrame : public wxFrame
{
private:
    static energonsoftware::Logger& logger;

public:
    virtual ~MainFrame() throw();

public:
    wxSize canvas_size() const;
    ConsolePanel* log_panel() const { return _log_panel; }

private:
    void OnMove(wxMoveEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

private:
    void create_ui();

    void create_menubar();
    void create_file_menu(wxMenuBar* menubar);
    void create_editor_menu(wxMenuBar* menubar);
    void create_view_menu(wxMenuBar* menubar);
    void create_debug_menu(wxMenuBar* menubar);
    void create_help_menu(wxMenuBar* menubar);
    void scene_load_progress_callback(float percent, const std::string& display);

private:
    friend class Application;
    DECLARE_EVENT_TABLE()

private:
    Canvas* _canvas;
    ConsolePanel* _log_panel;
    wxMenuItem *_file_close_item;
    wxMenuItem *_editor_render_bump_item, *_editor_render_vertex_item, *_editor_render_bounds, *_editor_render_wireframe,
        *_editor_render_shadows, *_editor_render_skeleton, *_editor_enable_lighting, *_editor_render_normals;
    wxMenuItem *_view_fullscreen_item;
    wxMenuItem *_debug_rotate_actors;

private:
    MainFrame();
    DISALLOW_COPY_AND_ASSIGN(MainFrame);
};

#endif
