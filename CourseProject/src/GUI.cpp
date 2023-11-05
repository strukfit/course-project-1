#include "GUI.h"

wxPanel* GUI::panel;

wxListBox* GUI::tableslistBox;

std::map<wxString, wxListCtrl*> GUI::tables;

std::map<wxString, wxCheckListBox*> GUI::checkBoxes;

using namespace std::string_literals;

void GUI::MainWindow(wxFrame* mainWindow, SQLController* sqlController)
{
    /*
    *   Binding the main window(mainWindow) with event processing.
    *   When closing the window, a confirmation window opens
    */
    mainWindow->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event)
        {
            if (event.CanVeto())
            {
                if (wxMessageBox("Are you sure you want to close?",
                    "Confirm action",
                    wxICON_QUESTION | wxYES_NO) != wxYES)
                {
                    event.Veto();
                    return;
                }
            }
            event.Skip();
        });

    // Creating menu bar
    wxMenuBar* menuBar = new wxMenuBar();

    // Creating file menu
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_NEW);
    fileMenu->Append(wxID_OPEN);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_SAVE);
    fileMenu->Append(wxID_SAVEAS);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    // Creating edit menu
    wxMenu* editMenu = new wxMenu();
    editMenu->Append(wxID_UNDO);
    editMenu->Append(wxID_REDO);
    editMenu->AppendSeparator();
    editMenu->Append(wxID_CUT);
    editMenu->Append(wxID_PASTE);

    menuBar->Append(fileMenu, "File");
    menuBar->Append(editMenu, "Edit");

    mainWindow->SetMenuBar(menuBar);

    // Creating a panel(A window on which controls are placed)
	panel = new wxPanel(mainWindow, wxID_ANY, wxDefaultPosition, wxSize(800, 600));
	panel->SetBackgroundColour(wxColour(43, 43, 43, 255));

    /*wxButton* button = new wxButton(panel, wxID_ANY, "Create DB", wxPoint(0, 0), wxSize(200, 50));
    button->SetBackgroundColour(wxColor(55, 55, 55, 255));
    button->SetForegroundColour(wxColor(*wxWHITE));
    button->Bind(wxEVT_BUTTON, &GUI::OnButtonClicked);*/
    
    tableslistBox = new wxListBox(panel, wxID_ANY, wxPoint(0, 40), wxSize(125, 50));
    
    // Getting all table names from database
    sqlite3_stmt* stmt = sqlController->PrepareSQL("SELECT name FROM sqlite_sequence");
    
    // Adding table and column names to the list box
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Getting table name  
        char* tableName = (char*)sqlite3_column_text(stmt, 0);

        // Adding table name to the list box
        tableslistBox->Append(tableName);
        tableslistBox->Bind(wxEVT_LISTBOX, &GUI::OnListBoxSelect);
        
        // Getting column names from the table
        sqlite3_stmt* stmt1 = sqlController->PrepareSQL(("SELECT name FROM pragma_table_info(\'"s + tableName + "\')").c_str());

        // Container that contains checkboxes
        std::vector<wxCheckBox*> checkBoxesArray;

        int yy = 100;
        wxCheckListBox* checkListBox = new wxCheckListBox(panel, wxID_ANY, wxPoint(10, 100), wxSize(115, 150));

        // Creating checkboxes of column names
        while (sqlite3_step(stmt1) == SQLITE_ROW) {
            
            // Creating checkbox listbox
            checkListBox->Append((char*)sqlite3_column_text(stmt1, 0));

            // Creating and adding visual table to the map
            GUI::TableInit(sqlController, stmt1, tableName);
        }

        checkListBox->Hide();

        // Insert checkbox and table name into the map
        checkBoxes.insert(std::make_pair(tableName, checkListBox));

        // Release resources associated with a prepared stmt SQL query
        sqlite3_finalize(stmt1);
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt);

    tableslistBox->SetBackgroundColour(wxColor(43, 43, 43, 255));
    tableslistBox->SetForegroundColour(wxColor(*wxWHITE));

    // Selecting the first table in tablesListBox
    tableslistBox->Select(0);

    // Showing the first table
    tables.begin()->second->Show();
    
    // Showing the the first table checkboxes
    checkBoxes.begin()->second->Show();

    mainWindow->CreateStatusBar();

    mainWindow->Show();
}

void GUI::TableInit(SQLController* sqlController, sqlite3_stmt* stmt, const char* tableName)
{
    // Creating visual table
    wxListCtrl* table = new wxListCtrl(panel, wxID_ANY, wxPoint(140, 40), wxSize(700, 300), wxLC_REPORT);

    // Selecting table from database
    sqlite3_stmt* stmt1 = sqlController->SelectData(tableName);

    // Adding column names to a table
    for (int i = 0; i < sqlite3_column_count(stmt1); ++i) {
        table->AppendColumn((char*)sqlite3_column_name(stmt1, i));
    }
        
    // Populating a table with database data
    while (sqlite3_step(stmt1) == SQLITE_ROW) {
        // Table row id
        int id = sqlite3_column_int(stmt1, 0);

        // Adding a new row to a table
        long index = table->InsertItem(table->GetItemCount(), wxString::Format("%d", id));
        for (int i = 0; i < sqlite3_column_count(stmt1); ++i) {
            const char* data = (char*)sqlite3_column_text(stmt1, i);
            table->SetItem(index, i, data);
        }
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt1);

    table->SetBackgroundColour(wxColor(31, 31, 31, 255));
    table->SetTextColour(wxColor(255, 255, 255, 255));

    table->Hide();

    // Insert table and it's name into the map
    tables.insert(std::make_pair(tableName, table));
}

void GUI::OnButtonClicked(wxCommandEvent& event)
{
	
}

void GUI::OnListBoxSelect(wxCommandEvent& event)
{
    // Getting an id of the selected table name in the listbox
    int id = tableslistBox->GetSelection();

    wxString tableName = tableslistBox->GetString(id);

    // Showing a selected table and hiding another
    for (auto it = tables.begin(); it != tables.end(); it++)
    {
        if (it->first == tableName)
        {
            it->second->Show();
        }
        else
        {
            it->second->Hide();
        }
    }
    
    // Showing checkboxes from a selected table and hiding another
    for (auto it = checkBoxes.begin(); it != checkBoxes.end(); it++)
    {
        if (it->first == tableName)
        {
            it->second->Show();
        }
        else
        {
            it->second->Hide();
        }
    }
}
