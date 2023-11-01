#pragma once

#include <wx/wx.h>
#include "SQLController.h"

/// <summary>
/// 
/// </summary>
struct Flags
{

};

/// <summary>
/// Implementation of a GUI(Graphical user interface)
/// </summary>
class GUI : public wxFrame
{	
	static void OnButtonClicked(wxCommandEvent& event);

public: 
	
	static void MainFrame(wxFrame* mainFrame);

	static void ErrorFrame(const char* errorName);

	static void addDataFrame(wxFrame* mainFrame);

	static void confirmationFrame(wxFrame* mainFrame);

};