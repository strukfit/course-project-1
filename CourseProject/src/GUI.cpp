#include "GUI.h"

void GUI::MainFrame(wxFrame* mainFrame)
{
	mainFrame->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event)
        {
            if (event.CanVeto())
            {
                if (wxMessageBox("Are you sure you want to close?",
                    "Confirm action",
                    wxICON_QUESTION | wxYES_NO) != wxYES)
                {
                    event.Veto();
                    return;
                }
            }
            event.Skip();   //вызовет Destroy/0 по-умолчанию  
        });
	wxPanel* panel = new wxPanel(mainFrame, wxID_ANY, wxDefaultPosition, wxSize(800, 600));
	panel->SetBackgroundColour(wxColour(43, 43, 43, 255));
	/*wxButton* button = new wxButton(panel, wxID_ANY, "Create DB", wxPoint(0, 0), wxSize(200, 50));
	button->SetBackgroundColour(wxColor(55, 55, 55, 255));
	button->SetForegroundColour(wxColor(*wxWHITE));

	button->Bind(wxEVT_BUTTON, &GUI::OnButtonClicked);*/

	mainFrame->Show();
}

void GUI::ErrorFrame(const char* errorName)
{
	//wxMessageDialog dlg(mainFrame, errorName);
}

void GUI::OnButtonClicked(wxCommandEvent& event)
{
	
}
