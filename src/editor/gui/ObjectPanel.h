#if !defined _OBJECTPANEL_H__
#define __OBJECTPANEL_H__

#include <wx/panel.h>

class ObjectPanel : public wxPanel
{
public:
    virtual ~ObjectPanel() throw();

private:
    friend class ControlPanel;

private:
    ObjectPanel();
    explicit ObjectPanel(wxWindow* parent);
    DISALLOW_COPY_AND_ASSIGN(ObjectPanel);
};

#endif
