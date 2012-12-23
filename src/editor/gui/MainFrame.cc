#include "src/pch.h"
#include <wx/wx.h>
#include <wx/splitter.h>
#include "src/engine/renderer/Light.h"
#include "src/editor/EditorConfiguration.h"
#include "controls.h"
#include "Canvas.h"
#include "ControlPanel.h"
#include "ConsolePanel.h"
#include "MainFrame.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MOVE(MainFrame::OnMove)
    EVT_SIZE(MainFrame::OnSize)
    EVT_MOUSE_CAPTURE_LOST(MainFrame::OnMouseCaptureLost)
END_EVENT_TABLE()

energonsoftware::Logger& MainFrame::logger(energonsoftware::Logger::instance("gled.editor.gui.MainFrame"));

MainFrame::MainFrame()
    : wxFrame(NULL, wxID_ANY, _("GLED Editor"), wxDefaultPosition, wxDefaultSize),
        _canvas(NULL), _log_panel(NULL), _file_close_item(NULL), _editor_render_bump_item(NULL), _editor_render_vertex_item(NULL),
        _editor_render_bounds(NULL), _editor_render_wireframe(NULL), _editor_render_shadows(NULL), _editor_render_skeleton(NULL),
        _editor_enable_lighting(NULL), _editor_render_normals(NULL), _view_fullscreen_item(NULL), _debug_rotate_actors(NULL)
{
    const EditorConfiguration& config(EditorConfiguration::instance());
    SetSize(config.editor_x(), config.editor_y(), config.editor_width(), config.editor_height());
    create_ui();

    SetAutoLayout(true);
    _canvas->SetFocus();
}

MainFrame::~MainFrame() throw()
{
}

wxSize MainFrame::canvas_size() const
{
    return _canvas->GetSize();
}

void MainFrame::OnMove(wxMoveEvent& event)
{
    EditorConfiguration& config(EditorConfiguration::instance());

    const wxPoint& position(event.GetPosition());
    config.editor_x(position.x);
    config.editor_y(position.y);

    event.Skip();
}

void MainFrame::OnSize(wxSizeEvent& event)
{
    EditorConfiguration& config(EditorConfiguration::instance());

    const wxSize& size(event.GetSize());
    config.editor_width(size.GetWidth());
    config.editor_height(size.GetHeight());

    event.Skip();
}

void MainFrame::OnMouseCaptureLost(wxMouseCaptureLostEvent& event)
{
    //ReleaseMouse();
}

void MainFrame::create_ui()
{
    create_menubar();
    CreateStatusBar();

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

    wxSplitterWindow* vsplitter = new wxSplitterWindow(this);
    vsplitter->SetMinimumPaneSize(10);

    wxSplitterWindow* hsplitter = new wxSplitterWindow(vsplitter);
    hsplitter->SetMinimumPaneSize(10);

    _canvas = new Canvas(hsplitter, Canvas::ATTRIB_LIST);
    _log_panel = new ConsolePanel(hsplitter);

    hsplitter->SplitHorizontally(_canvas, _log_panel, -150);
    vsplitter->SplitVertically(new ControlPanel(vsplitter), hsplitter, 300);

    sizer->Add(vsplitter, 1, wxEXPAND);
    SetSizer(sizer);
}

void MainFrame::create_menubar()
{
    wxMenuBar* menubar = new wxMenuBar();
    create_file_menu(menubar);
    create_editor_menu(menubar);
    create_view_menu(menubar);
    create_debug_menu(menubar);
    create_help_menu(menubar);
    SetMenuBar(menubar);
}

void MainFrame::create_file_menu(wxMenuBar* menubar)
{
    wxMenu* menu = new wxMenu();

    menu->Append(wxID_OPEN, _("&Open Scene..."), _("Open a scene file"));

    _file_close_item = menu->Append(wxID_CLOSE, _("&Close"), _("Close the scene file"));
    _file_close_item->Enable(false);

    menu->Append(wxID_EXIT, _("E&xit"), _("Exit"));

    menubar->Append(menu, _("&File"));
}

void MainFrame::create_editor_menu(wxMenuBar* menubar)
{
    wxMenu* menu = new wxMenu();

    menu->Append(ID_EDITOR_RESOURCES, _("&Resources..."));

    menu->AppendSeparator();

    wxMenu* sub_menu = new wxMenu();

    _editor_render_bump_item = sub_menu->AppendRadioItem(ID_EDITOR_RENDER_BUMP, _("&Bumpmap"));
    _editor_render_bump_item->Check(EditorConfiguration::instance().render_mode_bump());

    _editor_render_vertex_item = sub_menu->AppendRadioItem(ID_EDITOR_RENDER_VERTEX, _("&Vertex"));
    _editor_render_vertex_item->Check(EditorConfiguration::instance().render_mode_vertex());

    menu->AppendSubMenu(sub_menu, _("&Render Mode"));

    _editor_render_bounds = menu->AppendCheckItem(ID_EDITOR_RENDER_BOUNDS, _("Render &Bounds"));
    //_editor_render_bounds->Check(State::instance().render_bounds());

    _editor_render_wireframe = menu->AppendCheckItem(ID_EDITOR_RENDER_WIREFRAME, _("Render &Wireframe"));
    //_editor_render_wireframe->Check(State::instance().render_wireframe());

    _editor_render_shadows = menu->AppendCheckItem(ID_EDITOR_RENDER_SHADOWS, _("Render &Shadows"));
    _editor_render_shadows->Check(EditorConfiguration::instance().render_shadows());

    _editor_render_skeleton = menu->AppendCheckItem(ID_EDITOR_RENDER_SKELETON, _("Render S&keleton"));
    //_editor_render_skeleton->Check(State::instance().render_skeleton());

    _editor_enable_lighting = menu->AppendCheckItem(ID_EDITOR_ENABLE_LIGHTING, _("Enable &Lighting"));
    _editor_enable_lighting->Check(energonsoftware::Light::lighting_enabled());

    _editor_render_normals = menu->AppendCheckItem(ID_EDITOR_RENDER_NORMALS, _("Render &Normals"));
    //_editor_render_normals->Check(State::instance().render_normals());

    menubar->Append(menu, _("&Editor"));
}

void MainFrame::create_view_menu(wxMenuBar* menubar)
{
    wxMenu* menu = new wxMenu();

    _view_fullscreen_item = menu->Append(ID_VIEW_FULLSCREEN, _("&Fullscreen\tCtrl+F"), _("View fullscreen"));

    menubar->Append(menu, _("&View"));
}

void MainFrame::create_debug_menu(wxMenuBar* menubar)
{
    wxMenu* menu = new wxMenu();

    menu->Append(ID_DEBUG_MEMORY, _("Dump &Memory Details"));

    _debug_rotate_actors = menu->AppendCheckItem(ID_DEBUG_ROTATE_ACTORS, _("&Rotate Actors"));
    //_debug_rotate_actors->Check(State::instance().rotate_actors());

    menubar->Append(menu, _("&Debug"));
}

void MainFrame::create_help_menu(wxMenuBar* menubar)
{
    wxMenu* menu = new wxMenu();

    menu->Append(wxID_ABOUT, _("&About..."), _("About gled"));

    menubar->Append(menu, _("&Help"));
}

void MainFrame::scene_load_progress_callback(float percent, const std::string& display)
{
    LOG_INFO(display << "\n");
}
