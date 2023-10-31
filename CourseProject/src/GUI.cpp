#include "GUI.h"

void GUI::MainFrame(wxFrame* mainFrame)
{
	wxPanel* panel = new wxPanel(mainFrame, wxID_ANY, wxDefaultPosition, wxSize(800, 600));
	panel->SetBackgroundColour(wxColour(43, 43, 43, 255));
	wxButton* button = new wxButton(panel, wxID_ANY, "Create DB", wxPoint(0, 0), wxSize(200, 50));
	button->SetBackgroundColour(wxColor(55, 55, 55, 255));
	button->SetForegroundColour(wxColor(*wxWHITE));

	wxButton* button2 = new wxButton(panel, wxID_ANY, "Button", wxPoint(210, 0), wxSize(200, 50));
	button2->SetBackgroundColour(wxColor(55, 55, 55, 255));
	button2->SetForegroundColour(wxColor(*wxWHITE));

	button->Bind(wxEVT_BUTTON, &GUI::OnButtonClicked);
	//button2->Bind(wxEVT_BUTTON, &GUI::OnButtonClicked);
	mainFrame->Show();
}

void GUI::ErrorFrame(wxFrame* mainFrame, const char* errorName)
{
	//wxMessageDialog dlg(mainFrame, errorName);
}

void GUI::OnButtonClicked(wxCommandEvent& event)
{
	SQLController sqlController;

	try
	{
		sqlController.createDB("D:\\aboba\CPP Dev\\CourseProject\\out\\build\\x64-debug\\CourseProject\\database1.db");
	}
	catch (Exception exp)
	{
		wxLogMessage(exp.what());
	}

	/*SQLController sqlController("database.db");

	if (sqlController.executeSQL("CREATE TABLE IF NOT EXISTS MyTable (ID INTEGER PRIMARY KEY, Name TEXT)") == SQLITE_OK) {
		wxLogMessage("Таблица создана или уже существует.");
	}*/
}
