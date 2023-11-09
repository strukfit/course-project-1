#include "GUI.h"

SQLController* GUI::sqlController;

wxPanel* GUI::panel;

wxPanel* GUI::tablePanel;

wxListBox* GUI::tableslistBox;

std::map<wxString, wxDataViewListCtrl*> GUI::tables;

std::map<wxString, wxCheckListBox*> GUI::checkBoxes;

wxString GUI::selectedTable;

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
            if (wxMessageBox("Are you sure you want to close?",
                "Confirm action",
                wxICON_QUESTION | wxYES_NO) != wxYES)
            {
                event.Veto();
                return;
            }
            event.Skip();
        }
    );

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
    panel = new wxPanel(mainWindow, wxID_ANY, wxDefaultPosition, mainWindow->GetSize());
    panel->SetBackgroundColour(wxColour(43, 43, 43, 255));

    tablePanel = new wxPanel(panel, wxID_ANY, wxPoint(130, 30), wxSize(750, 350));

    tableslistBox = new wxListBox(panel, wxID_ANY, wxPoint(0, 40), wxSize(125, 50));
    
    //Getting all table names from database
    sqlite3_stmt* stmt = sqlController->PrepareSQL("SELECT name FROM sqlite_sequence");
    
    // Using data in every row of every database table
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

    //Selecting the first table in tablesListBox
    tableslistBox->Select(0);

    // Storing selected table name
    selectedTable = tableslistBox->GetString(0);

    // Showing the first table
    tables.begin()->second->Show();
    
    // Showing the the first table checkboxes
    checkBoxes.begin()->second->Show();

    //Creating button that check all checkboxes
    wxButton* checkAllButton = new wxButton(panel, wxID_ANY, "Check all", wxPoint(20, 260), wxSize(95, 25));
    checkAllButton->Bind(wxEVT_BUTTON, &GUI::OnCheckAllButtonClicked);
    
    // Creating update table button and handling event function
    wxButton* updateTableButton = new wxButton(panel, wxID_ANY, "Update Table", wxPoint(145, 425), wxSize(100, 25));
    updateTableButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
        {
            GUI::UpdateTable(selectedTable);
        }
    );

    // Creating add data button
    wxButton* addDataButton = new wxButton(panel, wxID_ANY, "Add", wxPoint(145, 380), wxSize(75, 25));
    addDataButton->Bind(wxEVT_BUTTON, &GUI::OnAddDataButtonClicked);

    // Creating update data button
    wxButton* updateDataButton = new wxButton(panel, wxID_ANY, "Update", wxPoint(225, 380), wxSize(75, 25));
    updateDataButton->Disable();

    // Creating delete data button
    wxButton* deleteDataButton = new wxButton(panel, wxID_ANY, "Delete", wxPoint(305, 380), wxSize(75, 25));
    deleteDataButton->Disable();

    mainWindow->CreateStatusBar();

    mainWindow->Show();
}

void GUI::TableInit(const char* tableName)
{
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    // Creating visual table
    wxDataViewListCtrl* table = new wxDataViewListCtrl(tablePanel, wxID_ANY);

    // Selecting table from database
    sqlite3_stmt* stmt = sqlController->SelectData(tableName);

    // Adding column names to a table
    for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
        wxString columnName = (char*)sqlite3_column_name(stmt, i);
        wxDataViewColumn* column = table->AppendTextColumn(columnName);
        column->SetMinWidth(columnName.Length() * 8); // Set a minimum width for the column
    }

    // Populating a table with database data
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Adding a new row to a table
        wxVector<wxVariant> data;
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            data.push_back(wxVariant(wxString::FromUTF8((char*)sqlite3_column_text(stmt, i))));
        }
        table->AppendItem(data);
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt);

    table->SetBackgroundColour(wxColor(31, 31, 31, 255));
    table->SetAlternateRowColour(wxColor(31, 31, 31, 255));
    table->SetForegroundColour(wxColor(*wxWHITE));

    // Добавляем `wxDataViewListCtrl` в вертикальный бокс-сайзер
    vbox->Add(table, 1, wxEXPAND | wxALL, 10);

    // Устанавливаем бокс-сайзер для панели
    tablePanel->SetSizer(vbox);

    tablePanel->Layout();

    table->Hide();

    // Insert table and it's name into the map
    tables.insert(std::make_pair(tableName, table));
}

void GUI::AddDataWindowInit(const char* tableName)
{
    
}

void GUI::UpdateTable(const char* tableName)
{
    wxDataViewListCtrl* table = tables.at(tableName);

    table->DeleteAllItems();

    // Selecting table from database
    sqlite3_stmt* stmt = sqlController->SelectData(tableName);

    // Populating a table with database data
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Adding a new row to a table
        wxVector<wxVariant> data;
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            data.push_back(wxVariant((char*)sqlite3_column_text(stmt, i)));
        }
        table->AppendItem(data);
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt);
}

void GUI::OnCheckBoxToggled(wxCommandEvent& event)
{
    wxCheckListBox* checkListBox = wxDynamicCast(event.GetEventObject(), wxCheckListBox);

    int id = event.GetInt();

    tables.at(selectedTable)->GetColumn(id)->SetHidden(!checkListBox->IsChecked(id));
    
    tablePanel->Layout();
}

void GUI::OnCheckAllButtonClicked(wxCommandEvent& event)
{
    wxCheckListBox* checkListBox = checkBoxes.at(selectedTable);
    for (int i = 0; i < checkListBox->GetCount(); i++)
    {
        checkListBox->Check(i);
        tables.at(selectedTable)->GetColumn(i)->SetHidden(false);
    }
    tablePanel->Layout();
}

void GUI::OnListBoxSelect(wxCommandEvent& event)
{
    // Getting an id of the selected table name in the listbox
    int id = event.GetSelection();

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

void GUI::OnAddDataButtonClicked(wxCommandEvent& event)
{
    AddDataDialog* addDataDialog = new AddDataDialog(panel, sqlController, selectedTable);
    addDataDialog->ShowModal();
    UpdateTable(selectedTable);
}

AddDataDialog::AddDataDialog(wxWindow* parent, SQLController* sqlController, wxString selectedTable)
    : wxDialog(parent, wxID_ANY, "Add data")
{
    this->sqlController = sqlController;

    this->selectedTable = selectedTable;

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Selecting table from database
    sqlite3_stmt* stmt = sqlController->SelectData(selectedTable);

    // Getting column names and adding to the form
    for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
        // Создайте элементы управления для ввода данных
        wxStaticText* columnName = new wxStaticText(this, wxID_ANY, (char*)sqlite3_column_name(stmt, i));
        wxTextCtrl* textCtrl = new wxTextCtrl(this, wxID_ANY);

        sizer->Add(columnName, 0, wxALL, 5);
        sizer->Add(textCtrl, 0, wxALL | wxEXPAND, 5);

        formFields.push_back(textCtrl);
    }

    // Кнопка для сохранения данны
    wxButton* saveButton = new wxButton(this, wxID_OK, "Save");
    saveButton->Bind(wxEVT_BUTTON, &AddDataDialog::OnSave, this);

    // Кнопка для отмены
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    buttonSizer->Add(saveButton, 0, wxALL | wxEXPAND, 5);
    buttonSizer->Add(cancelButton, 0, wxALL | wxEXPAND, 5);

    // Разместите элементы управления с использованием wxSizer
    sizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 5);

    SetSizer(sizer);
    sizer->Fit(this);
    sizer->SetSizeHints(this);
}

void AddDataDialog::OnSave(wxCommandEvent& event)
{
    // Getting column types from the table
    sqlite3_stmt* stmt = sqlController->PrepareSQL(("SELECT type FROM pragma_table_info(\'"s + selectedTable + "\')").c_str());
    
    std::string values = "";

    int totalRows = formFields.size() - 1;

    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        const char* type = (char*)sqlite3_column_text(stmt, 0);

        if (std::strcmp(type, "VARCHAR(255)") == 0 || std::strcmp(type, "TEXT") == 0)
        {
            values += "\""s + formFields[i]->GetValue() + "\"";
        }
        else
        {
            values += formFields[i]->GetValue();
        }

        if (i < totalRows)
        {
            values += ", ";

        }
        i++;
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt);

    sqlController->ExecuteSQL("INSERT INTO "s + selectedTable + " VALUES(" + values + ")");

    // Закройте окно после сохранения
    EndModal(wxID_OK);
}

