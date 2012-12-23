#if !defined __RESOURCEDIALOG_H__
#define __RESOURCEDIALOG_H__

#include <wx/dialog.h>

class ResourceDialog : public wxDialog
{
private:
    class LightResourcePanel : public wxPanel
    {
    public:
        explicit LightResourcePanel(wxWindow* parent);
        virtual ~LightResourcePanel() throw();

    private:
        LightResourcePanel();
        DISALLOW_COPY_AND_ASSIGN(LightResourcePanel);
    };

public:
    virtual ~ResourceDialog() throw();

private:
    friend class Application;

private:
    ResourceDialog();
    explicit ResourceDialog(wxWindow* parent);
    DISALLOW_COPY_AND_ASSIGN(ResourceDialog);
};

#endif
