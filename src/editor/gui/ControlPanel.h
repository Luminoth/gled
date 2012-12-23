#if !defined __CONTROLPANEL_H__
#define __CONTROLPANEL_H__

#include <wx/panel.h>

class ControlPanel : public wxPanel
{
public:
    virtual ~ControlPanel() throw();

private:
    friend class MainFrame;

private:
    ControlPanel();
    explicit ControlPanel(wxWindow* parent);
    DISALLOW_COPY_AND_ASSIGN(ControlPanel);
};

#endif
