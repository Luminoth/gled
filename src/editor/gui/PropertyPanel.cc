#include "src/pch.h"
#include <wx/wx.h>
#include "PropertyPanel.h"

PropertyPanel::PropertyPanel(wxWindow* parent)
    : wxPanel(parent)
{
    SetMinSize(wxSize(250, 100));

    wxSizer* sizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Properties"));
    SetSizer(sizer);
}

PropertyPanel::~PropertyPanel() throw()
{
}
