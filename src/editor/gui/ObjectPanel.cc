#include "src/pch.h"
#include <wx/wx.h>
#include "ObjectTreeView.h"
#include "ObjectPanel.h"

ObjectPanel::ObjectPanel(wxWindow* parent)
    : wxPanel(parent)
{
    SetMinSize(wxSize(250, 100));

    wxSizer* sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Objects"));
    sizer->Add(new ObjectTreeView(this), 1, wxEXPAND);
    SetSizer(sizer);
}

ObjectPanel::~ObjectPanel() throw()
{
}
