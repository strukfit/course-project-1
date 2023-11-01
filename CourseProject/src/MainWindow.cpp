#include "MainWindow.h"

//using namespace std::string_literals;
MainWindow::MainWindow():
	mainWindow(new wxFrame(nullptr, wxID_ANY, "Window", wxPoint(50, 50), wxSize(800, 600)))
{
	GUI::MainFrame(mainWindow);

	SQLController sqlController;
	try
	{
		sqlController.OpenDB("database.db");
		sqlController.DatabaseInit();
		GUI::ShowTable(sqlController, "Products");
		//wxLogStatus();
	}
	catch (Exception exc)
	{
		wxLogError(exc.what());
	}
}

