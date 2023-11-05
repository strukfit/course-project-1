#include "MainWindow.h"

MainWindow::MainWindow():
	mainWindow(new wxFrame(nullptr, wxID_ANY, "Household Appliances Store", wxPoint(50, 50), wxSize(900, 550), wxDEFAULT_FRAME_STYLE &~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)))
{
	SQLController* sqlController = new SQLController;
	try
	{
		sqlController->OpenDB("database.db");
		sqlController->DatabaseInit();
		GUI::MainWindowInit(mainWindow, sqlController);
		//wxLogStatus();
	}
	catch (Exception exc)
	{
		wxLogError(exc.what().c_str());
	}
}

