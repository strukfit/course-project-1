#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
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

	static wxPanel* panel;

public: 
	
	static void MainFrame(wxFrame* mainFrame);

	static void ErrorFrame(const char* errorName);

	static void AddDataFrame(wxFrame* mainFrame);

	static void ConfirmationFrame(wxFrame* mainFrame);

	static void ShowTable(SQLController sqlController, const char* tableName);

};