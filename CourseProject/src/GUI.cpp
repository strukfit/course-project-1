#include "GUI.h"

SQLController* GUI::sqlController;

wxPanel* GUI::panel;

wxListBox* GUI::tableslistBox;

std::map<wxString, wxListCtrl*> GUI::tables;

std::map<wxString, wxCheckListBox*> GUI::checkBoxes;

wxString GUI::selectedTable;

std::string GUI::checkedColumns = "*";

using namespace std::string_literals;

void GUI::MainWindowInit(wxFrame* mainWindow, SQLController* sql)
{

    sqlController = sql;

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

        wxCheckListBox* checkListBox = new wxCheckListBox(panel, wxID_ANY, wxPoint(10, 100), wxSize(115, 150));

        // Creating checkboxes of column names
        while (sqlite3_step(stmt1) == SQLITE_ROW) {
            
            // Creating checkbox listbox
            checkListBox->Check(checkListBox->Append((char*)sqlite3_column_text(stmt1, 0)));

            // Creating and adding visual table to the map
            GUI::TableInit(tableName);
        }

        checkListBox->Hide();

        checkListBox->Bind(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, &GUI::OnCheckBoxToggled);

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

    // Storing selected table name
    selectedTable = tableslistBox->GetString(0);

    // Showing the first table
    tables.begin()->second->Show();
    
    // Showing the the first table checkboxes
    checkBoxes.begin()->second->Show();

    // Creating button that check all checkboxes
    wxButton* checkAllButton = new wxButton(panel, wxID_ANY, "Check all", wxPoint(20, 260), wxSize(95, 25));
    checkAllButton->Bind(wxEVT_BUTTON, &GUI::OnCheckAllButtonClicked);
    
    // Creating update table button and handling event function
    wxButton* updateTableButton = new wxButton(panel, wxID_ANY, "Update Table", wxPoint(145, 10), wxSize(100, 25));
    updateTableButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
        {
            GUI::UpdateTable(selectedTable, checkedColumns);
        }
    );
    
    mainWindow->CreateStatusBar();

    mainWindow->Show();
}

void GUI::TableInit(const char* tableName)
{
    // Creating visual table
    wxListCtrl* table = new wxListCtrl(panel, wxID_ANY, wxPoint(140, 40), wxSize(700, 300), wxLC_REPORT);

    // Selecting table from database
    sqlite3_stmt* stmt = sqlController->SelectData(tableName);

    // Adding column names to a table
    for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
        table->AppendColumn((char*)sqlite3_column_name(stmt, i));
    }
    
    // Populating a table with database data
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Table row id
        int id = sqlite3_column_int(stmt, 0);

        // Adding a new row to a table
        long index = table->InsertItem(table->GetItemCount(), wxString::Format("%d", id));
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            const char* data = (char*)sqlite3_column_text(stmt, i);
            table->SetItem(index, i, data);
        }
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt);

    table->SetBackgroundColour(wxColor(31, 31, 31, 255));
    table->SetTextColour(wxColor(255, 255, 255, 255));

    table->Hide();

    // Insert table and it's name into the map
    tables.insert(std::make_pair(tableName, table));
}

void GUI::UpdateTable(const char* tableName, std::string columns)
{
    wxListCtrl* table = tables.at(tableName);

    table->DeleteAllColumns();
    table->DeleteAllItems();

    // Selecting table from database
    sqlite3_stmt* stmt = sqlController->SelectData(tableName, columns);

    // Adding column names to a table
    for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
        table->AppendColumn((char*)sqlite3_column_name(stmt, i));
    }

    // Populating a table with database data
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Table row id
        int id = sqlite3_column_int(stmt, 0);

        // Adding a new row to a table
        long index = table->InsertItem(table->GetItemCount(), wxString::Format("%d", id));
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            const char* data = (char*)sqlite3_column_text(stmt, i);
            table->SetItem(index, i, data);
        }
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt);
}

void GUI::OnCheckBoxToggled(wxCommandEvent& event)
{
    wxArrayInt checkedItems;
    wxCheckListBox* checkListBox = checkBoxes.at(selectedTable);
    checkListBox->GetCheckedItems(checkedItems);
    
    checkedColumns = "";

    for (int id : checkedItems)
    {
        checkedColumns += checkListBox->GetString(id);
        if (id < checkedItems.back()) {
            checkedColumns += ", ";
        }
    }
    UpdateTable(selectedTable, checkedColumns);
    
   /* wxString columnName = checkBoxes.at(selectedTable)->GetString(event.GetId());
    wxListCtrl* table = tables.at(selectedTable);*/
    
    //wxString toggledColumnName = event.GetString();

    //for (int i = 0; i < table->GetColumnCount(); i++)
    //{
    //    //if (table->GetColumn(i))
    //    wxListItem listItem;
    //    listItem.SetId(i);
    //    listItem.SetMask(wxLIST_MASK_TEXT);
    //    table->GetColumn(i, listItem);
    //    wxString columnName = listItem.GetText();
    //    wxMessageBox(columnName);

    //    if (columnName == toggledColumnName)
    //    {
    //        
    //    }
    //} 
    //if (table->GetColumn(i))
    /*wxListItem listItem;
    listItem.SetId(0);
    listItem.SetMask(wxLIST_MASK_TEXT);
    table->GetColumn(0, listItem);
    wxString columnName = listItem.GetText();
    listItem.SetWidth(0);

    event.IsChecked();*/
    //wxMessageBox(columnName);
}

void GUI::OnCheckAllButtonClicked(wxCommandEvent& event)
{
    wxCheckListBox* checkListBox = checkBoxes.at(selectedTable);
    for (int i = 0; i < checkListBox->GetCount(); i++)
    {
        checkListBox->Check(i);
    }
    checkedColumns = "*";
    GUI::UpdateTable(selectedTable, checkedColumns);
}

void GUI::OnListBoxSelect(wxCommandEvent& event)
{
    // Getting an id of the selected table name in the listbox
    int id = tableslistBox->GetSelection();

    selectedTable = tableslistBox->GetString(id);

    // Showing a selected table and hiding another
    for (auto it = tables.begin(); it != tables.end(); it++)
    {
        if (it->first == selectedTable)
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
        if (it->first == selectedTable)
        {
            it->second->Show();
        }
        else
        {
            it->second->Hide();
        }
    }
}
