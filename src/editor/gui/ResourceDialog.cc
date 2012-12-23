#include "src/pch.h"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include "src/engine/Engine.h"
#include "src/engine/ResourceManager.h"
#include "ResourceDialog.h"

ResourceDialog::LightResourcePanel::LightResourcePanel(wxWindow* parent)
    : wxPanel(parent)
{
    wxListView* listview = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    listview->InsertColumn(0, _("Light"));

    BOOST_FOREACH(const boost::filesystem::path& light, energonsoftware::Engine::instance().resource_manager().lights()) {
        listview->InsertItem(0, wxString(light.string().c_str(), wxConvUTF8));
    }

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(listview, 1, wxEXPAND);

    SetSizer(sizer);
}

ResourceDialog::LightResourcePanel::~LightResourcePanel() throw()
{
}

ResourceDialog::ResourceDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, _("Resources"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    SetMinSize(wxSize(350, 250));

    wxNotebook* notebook = new wxNotebook(this, wxID_ANY);
    notebook->AddPage(new LightResourcePanel(notebook), _("Lights"));

    wxSizer* vsizer = new wxBoxSizer(wxVERTICAL);
    vsizer->Add(notebook, 1, wxEXPAND);

    wxSizer* hsizer = new wxBoxSizer(wxHORIZONTAL);
    // TODO: add buttons or whatever

    vsizer->Add(hsizer);

    SetSizer(vsizer);
}

ResourceDialog::~ResourceDialog() throw()
{
}
