#include "MainWindow.h"

MainWindow::MainWindow():
	mainWindow(new wxFrame(nullptr, wxID_ANY, "Household Appliances Store", wxPoint(50, 50), wxSize(900, 550), wxDEFAULT_FRAME_STYLE &~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)))
{
	SQLController* sqlController = new SQLController;
	const char* dbName = "database.db";
	bool isDbExist = sqlController->isDatabaseExist(dbName);
	try
	{
		sqlController->OpenDB(dbName);
		if (!isDbExist)
		{
			sqlController->DatabaseInit();
		}
		GUI::MainWindowInit(mainWindow, sqlController);
	}
	catch (Exception exc)
	{
		wxLogError(exc.what().c_str());
	}
}
