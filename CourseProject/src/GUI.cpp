#include "GUI.h"

wxFrame* GUI::mainFrame;

void GUI::MainFrame()
{
	mainFrame = new wxFrame(nullptr, wxID_ANY, "Window", wxPoint(50, 50), wxSize(800, 600));
	wxPanel* panel = new wxPanel(mainFrame, wxID_ANY, wxDefaultPosition, wxSize(800, 600));
	panel->SetBackgroundColour(wxColour(43.f, 43.f, 43.f, 1.f));
	wxButton* button = new wxButton(panel, wxID_ANY, "Button", wxPoint(0, 0), wxSize(100, 35));
	button->SetBackgroundColour(wxColor(*wxWHITE));
	mainFrame->Show();
}
