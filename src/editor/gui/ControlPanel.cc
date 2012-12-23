#include "src/pch.h"
#include <wx/wx.h>
#include <wx/splitter.h>
#include "ObjectPanel.h"
#include "PropertyPanel.h"
#include "ControlPanel.h"

ControlPanel::ControlPanel(wxWindow* parent)
    : wxPanel(parent)
{
    SetMinSize(wxSize(250, 500));

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

    wxSplitterWindow* splitter = new wxSplitterWindow(this);
    splitter->SetMinimumPaneSize(10);
    splitter->SplitHorizontally(new ObjectPanel(splitter), new PropertyPanel(splitter), 500);

    sizer->Add(splitter, 1, wxEXPAND);
    SetSizer(sizer);
}

ControlPanel::~ControlPanel() throw()
{
}
