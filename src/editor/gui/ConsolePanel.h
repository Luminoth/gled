#if !defined __LOGPANEL_H__
#define __LOGPANEL_H__

#include <wx/panel.h>

class wxTextCtrl;

class ConsolePanel : public wxPanel
{
public:
    virtual ~ConsolePanel() throw();

public:
    void bind_output();
    void unbind_output();

private:
    friend class MainFrame;
    DECLARE_EVENT_TABLE()

private:
    wxTextCtrl *_output, *_input;

private:
    ConsolePanel();
    explicit ConsolePanel(wxWindow* parent);
    DISALLOW_COPY_AND_ASSIGN(ConsolePanel);
};

#endif
