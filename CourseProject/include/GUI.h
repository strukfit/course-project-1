#pragma once

#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/checkbox.h>
#include <wx/listctrl.h>
#include <wx/dataview.h>
#include <map>
#include "SQLController.h"

/// <summary>
/// Implementation of a GUI(Graphical user interface)
/// </summary>
class GUI : public wxFrame
{	

	static void AddDataWindow(wxFrame* mainWindow);

	/// <summary>
	/// Tables initialization. Creating and hiding visual tables 
	/// </summary>
	/// <param name="tableName"></param>
	static void TableInit(const char* tableName);

	/// <summary>
	/// Table update
	/// </summary>
	/// <param name="tableName"></param>
	static void UpdateTable(const char* tableName);

	/// <summary>
	/// Handling an event when a checkbox is clicked
	/// </summary>
	/// <param name="event - event for handling"></param>
	static void OnCheckBoxToggled(wxCommandEvent& event);

	/// <summary>
	/// Handling an event when a check all checkboxes button is clicked
	/// </summary>
	/// <param name="event - event for handling"></param>
	static void OnCheckAllButtonClicked(wxCommandEvent& event);

	/// <summary>
	/// Handling an event when a listbox string(table name) is selected
	/// </summary>
	/// <param name="event - event for handling"></param>
	static void OnListBoxSelect(wxCommandEvent& event);

	static SQLController* sqlController;

	static wxPanel* panel; // A panel(A window on which controls are placed)

	static wxPanel* tablePanel; // A panel(A window on which controls are placed)
	
	static wxListBox* tableslistBox; // A tableListBox is used to select one of a list of table names.

	/// <summary>
	/// Sorted associative container that contains wxDataViewListCtrl* table and its unique key(table name)
	/// </summary>
	static std::map<wxString, wxDataViewListCtrl*> tables;

	static wxString selectedTable; // Stores the name of the selected table 

	/// <summary>
	/// Sorted associative container that contains wxCheckListBox* objects(listbox of checkboxes) and its unique key(table name)
	/// </summary>
	static std::map<wxString, wxCheckListBox*> checkBoxes;

public: 
	
	/// <summary>
	/// Initializing the GUI of the main window
	/// </summary>
	/// <param name="mainWindow - main window(wxFrame class object)"></param>
	/// <param name="sqlController"></param>
	static void MainWindowInit(wxFrame* mainWindow, SQLController* sqlController);

};