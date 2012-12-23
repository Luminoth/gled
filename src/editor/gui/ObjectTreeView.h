#if !defined __OBJECTTREEVIEW_H__
#define __OBJECTTREEVIEW_H__

#include <wx/treectrl.h>

class ObjectTreeView : public wxTreeCtrl
{
public:
    virtual ~ObjectTreeView() throw();

private:
    friend class ObjectPanel;
    DECLARE_EVENT_TABLE()

private:
    wxTreeItemId _actors, _lights;

private:
    ObjectTreeView();
    explicit ObjectTreeView(wxWindow* parent);
    DISALLOW_COPY_AND_ASSIGN(ObjectTreeView);
};

#endif
