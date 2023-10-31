#include "MainWindow.h"

MainWindow::MainWindow():
	mainWindow(new wxFrame(nullptr, wxID_ANY, "Window", wxPoint(50, 50), wxSize(800, 600)))
{
	GUI::MainFrame(mainWindow);
	GUI::ErrorFrame(mainWindow, "Error");
}

