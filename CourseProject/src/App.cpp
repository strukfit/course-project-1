// CourseProject.cpp : Defines the entry point for the application.
//

#include "App.h"

bool App::OnInit()
{
	GUI::MainFrame();
	return true;
}

// Entry point
wxIMPLEMENT_APP(App);