#include "src/pch.h"
#include <wx/wx.h>
#include "ObjectTreeView.h"

BEGIN_EVENT_TABLE(ObjectTreeView, wxTreeCtrl)
END_EVENT_TABLE()

ObjectTreeView::ObjectTreeView(wxWindow* parent)
    : wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT)
{
    SetMinSize(wxSize(250, 100));

    wxTreeItemId root = AddRoot(_("Objects"));
    _actors = AppendItem(root, _("Actors"));
    _lights = AppendItem(root, _("Lights"));
}

ObjectTreeView::~ObjectTreeView() throw()
{
}
