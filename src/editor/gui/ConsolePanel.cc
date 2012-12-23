#include "src/pch.h"
#include <wx/wx.h>
#include "ConsolePanel.h"

BEGIN_EVENT_TABLE(ConsolePanel, wxPanel)
END_EVENT_TABLE()

ConsolePanel::ConsolePanel(wxWindow* parent)
    : wxPanel(parent), _output(NULL), _input(NULL)
{
    SetMinSize(wxSize(250, 50));

    _output = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_RICH2 | wxTE_AUTO_URL | wxHSCROLL);

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(_output, 1, wxEXPAND);

    _input = new wxTextCtrl(this, wxID_ANY, _(""), wxDefaultPosition, wxDefaultSize,
        wxTE_PROCESS_ENTER /*| wxTE_PROCESS_TAB*/ | wxHSCROLL);
    sizer->Add(_input, 0, wxEXPAND);

    SetSizer(sizer);
}

void ConsolePanel::bind_output()
{
    std::cout.rdbuf(_output);
    std::cerr.rdbuf(_output);
}

void ConsolePanel::unbind_output()
{
    std::cout.rdbuf(NULL);
    std::cerr.rdbuf(NULL);
}

ConsolePanel::~ConsolePanel() throw()
{
}
