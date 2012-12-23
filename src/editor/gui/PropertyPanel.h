#if !defined __PROPERTYPANEL_H__
#define __PROPERTYPANEL_H__

#include <wx/panel.h>

class PropertyPanel : public wxPanel
{
public:
    virtual ~PropertyPanel() throw();

private:
    friend class ControlPanel;

private:
    PropertyPanel();
    explicit PropertyPanel(wxWindow* parent);
    DISALLOW_COPY_AND_ASSIGN(PropertyPanel);
};

#endif
