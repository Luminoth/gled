#include "src/pch.h"
#include <signal.h>
#include <wx/wx.h>
#include <wx/cmdline.h>
#include "src/core/common.h"
#include "src/engine/Engine.h"
#include "src/engine/renderer/Light.h"
#include "src/engine/renderer/Renderer.h"
#include "gui/controls.h"
#include "gui/Canvas.h"
#include "gui/ConsolePanel.h"
#include "gui/MainFrame.h"
#include "gui/ResourceDialog.h"
#include "EditorConfiguration.h"
#include "EditorUIController.h"
#include "Application.h"

#include "src/engine/State.h"
#include "src/engine/scene/Actor.h"
#include "src/engine/scene/Scene.h"
#include "src/engine/renderer/Camera.h"
#include "src/game/Player.h"

IMPLEMENT_APP(Application)

BEGIN_EVENT_TABLE(Application, wxApp)
    EVT_IDLE(Application::OnIdle)
    EVT_MENU(wxID_OPEN, Application::OnFileOpen)
    EVT_MENU(wxID_CLOSE, Application::OnFileClose)
    EVT_MENU(wxID_EXIT, Application::OnFileExit)
    EVT_MENU(ID_EDITOR_RESOURCES, Application::OnEditorResources)
    EVT_MENU(ID_EDITOR_RENDER_BUMP, Application::OnEditorRenderBump)
    EVT_MENU(ID_EDITOR_RENDER_VERTEX, Application::OnEditorRenderVertex)
    EVT_MENU(ID_EDITOR_RENDER_BOUNDS, Application::OnEditorRenderBounds)
    EVT_MENU(ID_EDITOR_RENDER_WIREFRAME, Application::OnEditorRenderWireframe)
    EVT_MENU(ID_EDITOR_RENDER_SHADOWS, Application::OnEditorRenderShadows)
    EVT_MENU(ID_EDITOR_RENDER_SKELETON, Application::OnEditorRenderSkeleton)
    EVT_MENU(ID_EDITOR_ENABLE_LIGHTING, Application::OnEditorEnableLighting)
    EVT_MENU(ID_EDITOR_RENDER_NORMALS, Application::OnEditorRenderNormals)
    EVT_MENU(ID_VIEW_FULLSCREEN, Application::OnViewFullscreen)
    EVT_MENU(ID_DEBUG_MEMORY, Application::OnDebugVideoMemory)
    EVT_MENU(ID_DEBUG_ROTATE_ACTORS, Application::OnDebugRotateActors)
    EVT_MENU(wxID_ABOUT, Application::OnHelpAbout)
END_EVENT_TABLE()

energonsoftware::Logger& Application::logger(energonsoftware::Logger::instance("gled.editor.Application"));

void Application::signal_handler(int signum)
{
    if(signum == SIGINT) {
        LOG_INFO("Caught SIGINT, quitting...\n");
        energonsoftware::Engine::instance().quit();
    }
}

Application::Application()
    : wxApp(), _config_filename(energonsoftware::editor_conf()),
        _main_frame(NULL)
{
}

Application::~Application() throw()
{
}

bool Application::initialize_config_directory()
{
    if(!boost::filesystem::exists(energonsoftware::home_conf_dir())) {
        try {
            std::cout << "Creating home configuration directory " << energonsoftware::home_conf_dir() << std::endl;
            boost::filesystem::create_directory(energonsoftware::home_conf_dir());
        } catch(const boost::filesystem::filesystem_error& e) {
            std::cerr << "Could not create configuration directory: " << e.what() << std::endl;
            return false;
        }
    }

    return true;
}

bool Application::initialize_configuration()
{
    std::cout << "Initializing configuration..." << std::endl;
    EditorConfiguration& config(EditorConfiguration::instance());

    std::cout << "Reading configuration from '" << _config_filename << "'..." << std::endl;
    bool ret = config.load(_config_filename);
    config.validate();

    return ret;
}

void Application::save_configuration()
{
    std::cout << "Saving configuration to '" << _config_filename << "'..." << std::endl;

    EditorConfiguration& config(EditorConfiguration::instance());
    config.save(_config_filename);
}

bool Application::initialize_logger()
{
    const EditorConfiguration& config(EditorConfiguration::instance());
    if(!energonsoftware::Logger::configure(config.logging_type(), config.logging_level(), config.logging_filename())) {
        return false;
    }

    LOG_INFO("Logger initialized!\n");

    return true;
}

void Application::initialize_signal_handlers()
{
    LOG_INFO("Initializing signal handlers...\n");

    signal(SIGINT, signal_handler);
}

bool Application::OnInit()
{
    if(!wxApp::OnInit()) {
        return false;
    }

    if(!initialize_config_directory()) {
        return false;
    }

    try {
        if(!initialize_configuration()) {
            std::cerr << "WARNING: Reading configuration failed!" << std::endl;
            save_configuration();
        }
    } catch(const energonsoftware::ConfigurationError& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return false;
    }

    const EditorConfiguration& config(EditorConfiguration::instance());

    if(!initialize_logger()) {
        wxMessageBox(_("Could not initialize logger!"), _T("Error"), wxOK | wxICON_ERROR);
        return false;
    }

#if USE_SSE
    LOG_INFO("Using SSE\n");
#endif
    config.dump(logger);

    // initialize the signal handlers
    initialize_signal_handlers();

    Canvas::ATTRIB_LIST[Canvas::AttribColorDepth] = config.video_depth();
    _main_frame = new MainFrame();
    _main_frame->log_panel()->bind_output();
    _main_frame->Show();
    SetTopWindow(_main_frame);

    return true;
}

void Application::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);
}

bool Application::OnCmdLineParsed(wxCmdLineParser& parser)
{
    return wxApp::OnCmdLineParsed(parser);
}

int Application::OnRun()
{
    const EditorConfiguration& config(EditorConfiguration::instance());

    const wxSize& size(_main_frame->canvas_size());
    if(!energonsoftware::Engine::instance().init(size.GetWidth(), size.GetHeight(), config.video_depth())) {
        wxMessageBox(_("Engine::init() failed!"), _("Error"), wxOK | wxICON_ERROR);
        return 1;
    }

    // UI controller goes on the system allocator
    energonsoftware::MemoryAllocator& allocator(energonsoftware::Engine::instance().system_allocator());
    energonsoftware::UIController::controller(boost::shared_ptr<energonsoftware::UIController>(new(16, allocator) EditorUIController(),
        boost::bind(EditorUIController::destroy, _1, &allocator)));

    // TODO: is this even necessary??
    energonsoftware::Engine::instance().renderer().resize_viewport(size.GetWidth(), size.GetHeight(), config.game_fov());
    return wxApp::OnRun();
}

int Application::OnExit()
{
    _main_frame->log_panel()->unbind_output();
    energonsoftware::Engine::instance().shutdown();
    save_configuration();

    return wxApp::OnExit();
}

void Application::OnIdle(wxIdleEvent& event)
{
    if(energonsoftware::Engine::instance().should_quit()) {
        _main_frame->Close();
        return;
    }

    if(!energonsoftware::Engine::instance().ready()) {
        return;
    }

    /*Player::instance()->think();*/
    _main_frame->Refresh();
    energonsoftware::Engine::instance().finish_frame();

    event.RequestMore();
}

void Application::OnFileOpen(wxCommandEvent& event)
{
    wxFileDialog dlg(_main_frame, _("Open Scene"),
        wxString(energonsoftware::scene_dir().string().c_str(), wxConvUTF8), wxEmptyString/*_("default.scene")*/, _("Scene files (*.scene)|*.scene"),
        wxFD_DEFAULT_STYLE | wxFD_FILE_MUST_EXIST);
    if(wxID_CANCEL == dlg.ShowModal()) {
        return;
    }

////////
// TODO: move this stuff into the Engine, the application should only interact with that class
    energonsoftware::Engine::instance().state().scene().load_progress_callback(boost::bind(&MainFrame::scene_load_progress_callback, boost::ref(*_main_frame), _1, _2));
    boost::filesystem::path filename(std::string(dlg.GetPath().mb_str()));
    if(!energonsoftware::Engine::instance().state().scene().load(filename)) {
        LOG_ERROR("Error loading the scene!\n");
        return;
    }
    energonsoftware::Engine::instance().state().scene().register_physical(Player::instance());
    energonsoftware::Engine::instance().state().scene().camera().attach(Player::instance());
////////

    _main_frame->SetTitle(_("GLED - ") + dlg.GetFilename());
    _main_frame->_file_close_item->Enable(true);
}

void Application::OnFileClose(wxCommandEvent& event)
{
////////
// TODO: move this into the Engine
    energonsoftware::Engine::instance().state().scene().unload();
    energonsoftware::Engine::instance().state().scene().camera().detach();
////////

    _main_frame->SetTitle(_("GLED"));
    _main_frame->_file_close_item->Enable(false);
}

void Application::OnFileExit(wxCommandEvent& event)
{
    _main_frame->Close(true);
}

void Application::OnEditorResources(wxCommandEvent& event)
{
    ResourceDialog* dlg = new ResourceDialog(_main_frame);
    dlg->ShowModal();
    dlg->Destroy();
}

void Application::OnEditorRenderBump(wxCommandEvent& event)
{
    EditorConfiguration::instance().render_mode("bump");
    //_editor_render_bump_item->Check(EditorConfiguration::instance().render_mode_bump());
}

void Application::OnEditorRenderVertex(wxCommandEvent& event)
{
    EditorConfiguration::instance().render_mode("vertex");
    //_editor_render_vertex_item->Check(EditorConfiguration::instance().render_mode_vertex());
}

void Application::OnEditorRenderBounds(wxCommandEvent& event)
{
    EditorConfiguration& config(EditorConfiguration::instance());
    config.render_bounds(!config.render_bounds());
    _main_frame->_editor_render_bounds->Check(config.render_bounds());
}

void Application::OnEditorRenderWireframe(wxCommandEvent& event)
{
    EditorConfiguration& config(EditorConfiguration::instance());
    config.render_wireframe(!config.render_wireframe());

    boost::shared_ptr<energonsoftware::PolygonModeRenderCommand> command(
        boost::dynamic_pointer_cast<energonsoftware::PolygonModeRenderCommand, energonsoftware::RenderCommand>(
            energonsoftware::RenderCommand::new_render_command(energonsoftware::RenderCommand::RC_POLYGON_MODE)));
    command->face = GL_FRONT_AND_BACK;
    command->mode = config.render_wireframe() ? GL_LINE : GL_FILL;
    energonsoftware::Engine::instance().renderer().command_queue().push(command);

    _main_frame->_editor_render_wireframe->Check(config.render_wireframe());
}

void Application::OnEditorRenderShadows(wxCommandEvent& event)
{
    EditorConfiguration::instance().render_shadows(!EditorConfiguration::instance().render_shadows());
    _main_frame->_editor_render_shadows->Check(EditorConfiguration::instance().render_shadows());
}

void Application::OnEditorRenderSkeleton(wxCommandEvent& event)
{
    EditorConfiguration& config(EditorConfiguration::instance());
    config.render_skeleton(!config.render_skeleton());
    _main_frame->_editor_render_skeleton->Check(config.render_skeleton());
}

void Application::OnEditorEnableLighting(wxCommandEvent& event)
{
    energonsoftware::Light::lighting_enable(!energonsoftware::Light::lighting_enabled());
    _main_frame->_editor_enable_lighting->Check(energonsoftware::Light::lighting_enabled());
}

void Application::OnEditorRenderNormals(wxCommandEvent& event)
{
    EditorConfiguration& config(EditorConfiguration::instance());
    config.render_normals(!config.render_normals());
    _main_frame->_editor_render_normals->Check(config.render_normals());
}

void Application::OnViewFullscreen(wxCommandEvent& event)
{
    _main_frame->ShowFullScreen(!_main_frame->IsFullScreen());
}

void Application::OnDebugVideoMemory(wxCommandEvent& event)
{
    energonsoftware::Engine::instance().print_memory_details();
}

void Application::OnDebugRotateActors(wxCommandEvent& event)
{
    EditorConfiguration& config(EditorConfiguration::instance());
    config.rotate_actors(!config.rotate_actors());
    _main_frame->_debug_rotate_actors->Check(config.rotate_actors());
}

void Application::OnHelpAbout(wxCommandEvent& event)
{
    wxString title(_main_frame->GetTitle());
    wxMessageDialog* dlg = new wxMessageDialog(_main_frame, title, _("About ") + title, wxOK);
    dlg->ShowModal();
    dlg->Destroy();
}
