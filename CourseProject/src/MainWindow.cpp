#include "MainWindow.h"

//using namespace std::string_literals;
MainWindow::MainWindow():
	mainWindow(new wxFrame(nullptr, wxID_ANY, "Household Appliances Store", wxPoint(50, 50), wxSize(900, 550)))
{
	SQLController* sqlController = new SQLController;
	try
	{
		sqlController->OpenDB("database.db");
		sqlController->DatabaseInit();
		GUI::MainWindow(mainWindow, sqlController);
		//wxLogStatus();
	}
	catch (Exception exc)
	{
		wxLogError(exc.what().c_str());
	}
}

