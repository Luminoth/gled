#if !defined __APPLICATION_H__
#define __APPLICATION_H__

#include <wx/app.h>

class MainFrame;
class wxCmdLineParser;
class wxCommandEvent;
class wxIdleEvent;

class Application : public wxApp
{
private:
    static energonsoftware::Logger& logger;

private:
    static void signal_handler(int signum);

public:
    Application();
    virtual ~Application() throw();

private:
    bool initialize_config_directory();
    bool initialize_configuration();
    void save_configuration();
    bool initialize_logger();
    void initialize_signal_handlers();

    virtual bool OnInit();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
    virtual int OnRun();
    virtual int OnExit();

    void OnIdle(wxIdleEvent& event);

    void OnFileOpen(wxCommandEvent& event);
    void OnFileClose(wxCommandEvent& event);
    void OnFileExit(wxCommandEvent& event);
    void OnEditorResources(wxCommandEvent& event);
    void OnEditorRenderBump(wxCommandEvent& event);
    void OnEditorRenderVertex(wxCommandEvent& event);
    void OnEditorRenderBounds(wxCommandEvent& event);
    void OnEditorRenderWireframe(wxCommandEvent& event);
    void OnEditorRenderShadows(wxCommandEvent& event);
    void OnEditorRenderSkeleton(wxCommandEvent& event);
    void OnEditorEnableLighting(wxCommandEvent& event);
    void OnEditorRenderNormals(wxCommandEvent& event);
    void OnViewFullscreen(wxCommandEvent& event);
    void OnDebugVideoMemory(wxCommandEvent& event);
    void OnDebugRotateActors(wxCommandEvent& event);
    void OnHelpAbout(wxCommandEvent& event);

private:
    boost::filesystem::path _config_filename;
    MainFrame* _main_frame;
    DECLARE_EVENT_TABLE()

private:
    DISALLOW_COPY_AND_ASSIGN(Application);
};

DECLARE_APP(Application)

#endif
