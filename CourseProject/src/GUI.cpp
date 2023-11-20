#include "GUI.h"

SQLController* GUI::sqlController;

wxPanel* GUI::panel;

wxPanel* GUI::tablePanel;

wxListBox* GUI::tableslistBox;

std::map<wxString, wxDataViewListCtrl*> GUI::tables;

std::map<wxString, wxCheckListBox*> GUI::checkBoxes;

wxString GUI::selectedTable;

wxButton* GUI::updateDataButton;

wxButton* GUI::deleteDataButton;

AboutProgramDialog* GUI::aboutProgramDialog;

AboutDeveloperDialog* GUI::aboutDeveloperDialog;

bool GUI::rowSelected = false;

using namespace std::string_literals;

namespace MenuIds
{
    const int addDataId = 1;
    const int updateDataId = 2;
    const int deleteDataId = 3;

    const int aboutProgramId = 4;
    const int aboutDeveloperId = 5;
}

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

    // Creating database menu
    wxMenu* databaseMenu = new wxMenu();
    databaseMenu->Append(MenuIds::addDataId, "Add data \tCtrl-A");
    databaseMenu->Bind(wxEVT_MENU, GUI::OnAddDataButtonClicked, MenuIds::addDataId);

    databaseMenu->Append(MenuIds::updateDataId, "Update data \tCtrl-U");
    databaseMenu->Bind(wxEVT_MENU, GUI::OnUpdateDataButtonClicked, MenuIds::updateDataId);

    databaseMenu->Append(MenuIds::deleteDataId, "Delete data \tDel");
    databaseMenu->Bind(wxEVT_MENU, GUI::OnDeleteDataButtonClicked, MenuIds::deleteDataId);

    // Creating info menu
    wxMenu* infoMenu = new wxMenu();
    infoMenu->Append(MenuIds::aboutProgramId, "About program");
    aboutProgramDialog = new AboutProgramDialog(mainWindow);
    infoMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& event) { aboutProgramDialog->ShowModal(); }, MenuIds::aboutProgramId);
    
    infoMenu->Append(MenuIds::aboutDeveloperId, "About developer");
    aboutDeveloperDialog = new AboutDeveloperDialog(mainWindow);
    infoMenu->Bind(wxEVT_MENU, [&](wxCommandEvent& event) { aboutDeveloperDialog->ShowModal(); }, MenuIds::aboutDeveloperId);

    menuBar->Append(databaseMenu, "Database");
    menuBar->Append(infoMenu, "Info");

    mainWindow->SetMenuBar(menuBar);

    // Creating a panel(A window on which controls are placed)
    panel = new wxPanel(mainWindow, wxID_ANY, wxDefaultPosition, mainWindow->GetSize());
    panel->SetBackgroundColour(wxColour(43, 43, 43, 255));

    tablePanel = new wxPanel(panel, wxID_ANY, wxPoint(130, 10), wxSize(750, 350));

    tableslistBox = new wxListBox(panel, wxID_ANY, wxPoint(0, 20), wxSize(125, 50));
    
    //Getting all table names from database
    sqlite3_stmt* stmt = sqlController->PrepareSQL(R"(
        SELECT name
        FROM sqlite_master
        WHERE type = 'table'
        AND name != 'sqlite_sequence';
    )");
    
    // Using data in every row of every database table
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Getting table name  
        char* tableName = (char*)sqlite3_column_text(stmt, 0);

        // Adding table name to the list box
        tableslistBox->Append(tableName);
        tableslistBox->Bind(wxEVT_LISTBOX, &GUI::OnListBoxSelect);

        // Getting column names from the table
        sqlite3_stmt* stmt1 = sqlController->PrepareSQL(("SELECT name FROM pragma_table_info(\'"s + tableName + "\')").c_str());

        wxCheckListBox* checkListBox = new wxCheckListBox(panel, wxID_ANY, wxPoint(10, 80), wxSize(115, 125));

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

    // Showing selected table
    tables.at(selectedTable)->Show();
    
    // Showing selected table checkboxes
    checkBoxes.at(selectedTable)->Show();

    //Creating button that check all checkboxes
    wxButton* checkAllButton = new wxButton(panel, wxID_ANY, "Check all", wxPoint(20, 215), wxSize(95, 25));
    checkAllButton->Bind(wxEVT_BUTTON, &GUI::OnCheckAllButtonClicked);

    // Creating add data button
    wxButton* addDataButton = new wxButton(panel, wxID_ANY, "Add", wxPoint(145, 360), wxSize(75, 25));
    addDataButton->Bind(wxEVT_BUTTON, &GUI::OnAddDataButtonClicked);

    // Creating update data button
    updateDataButton = new wxButton(panel, wxID_ANY, "Update", wxPoint(225, 360), wxSize(75, 25));
    updateDataButton->Bind(wxEVT_BUTTON, &GUI::OnUpdateDataButtonClicked);
    updateDataButton->Disable();

    // Creating delete data button
    deleteDataButton = new wxButton(panel, wxID_ANY, "Delete", wxPoint(305, 360), wxSize(75, 25));
    deleteDataButton->Bind(wxEVT_BUTTON, &GUI::OnDeleteDataButtonClicked);
    deleteDataButton->Disable();

    // Creating total info button
    wxButton* totalInfoButton = new wxButton(panel, wxID_ANY, "Total", wxPoint(765, 360), wxSize(100, 25));
    totalInfoButton->Bind(wxEVT_BUTTON, &GUI::OnTotalInfoButtonClicked);
}

void GUI::TableInit(const char* tableName)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Creating visual table
    wxDataViewListCtrl* table = new wxDataViewListCtrl(tablePanel, wxID_ANY);

    // Selecting table from database
    sqlite3_stmt* stmt = sqlController->SelectData(tableName);

    // Adding column names to a table
    for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
        wxString columnName = wxString::FromUTF8((char*)sqlite3_column_name(stmt, i));
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

    // Add table to the vertical box-sizer
    sizer->Add(table, 1, wxEXPAND | wxALL, 10);

    // Set the box sizer for the panel
    tablePanel->SetSizer(sizer);

    tablePanel->Layout();

    table->Hide();

    table->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &GUI::OnSelectionChanged);

    // Insert table and it's name into the map
    tables.insert(std::make_pair(tableName, table));
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
            data.push_back(wxVariant(wxString::FromUTF8((char*)sqlite3_column_text(stmt, i))));
        }
        table->AppendItem(data);
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt);
}

void GUI::SetTableColumnWidths(wxDataViewListCtrl* table)
{
    for (size_t i = 0; i < table->GetColumnCount(); ++i) 
    {
        auto width = table->GetColumn(i)->GetTitle().Length();

        if (width < 10)
        {
            width *= 12;
        }
        else
        {
            width *= 8;
        }

        table->GetColumn(i)->SetWidth(width);
    }
}

void GUI::OnCheckBoxToggled(wxCommandEvent& event)
{
    wxCheckListBox* checkListBox = wxDynamicCast(event.GetEventObject(), wxCheckListBox);

    int id = event.GetInt();

    wxDataViewListCtrl* table = tables.at(selectedTable);
    table->GetColumn(id)->SetHidden(!checkListBox->IsChecked(id));

    SetTableColumnWidths(table);
    
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

    wxDataViewListCtrl* table = tables.at(selectedTable);
    
    SetTableColumnWidths(table);

    tablePanel->Layout();
}

void GUI::OnListBoxSelect(wxCommandEvent& event)
{
    updateDataButton->Disable();
    deleteDataButton->Disable();

    rowSelected = false;

    tables.at(selectedTable)->UnselectAll();

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
    AddDataDialog* addDataDialog = new AddDataDialog(panel, sqlController, selectedTable, tables.at(selectedTable));
    addDataDialog->ShowModal();
    UpdateTable(selectedTable);

    updateDataButton->Disable();
    deleteDataButton->Disable();
}

void GUI::OnUpdateDataButtonClicked(wxCommandEvent& event)
{
    if (rowSelected)
    {
        UpdateDataDialog* updateDataDialog = new UpdateDataDialog(panel, sqlController, selectedTable, tables.at(selectedTable));
        updateDataDialog->ShowModal();
        UpdateTable(selectedTable);

        updateDataButton->Disable();
        deleteDataButton->Disable();

        rowSelected = false;
    }
}

void GUI::OnDeleteDataButtonClicked(wxCommandEvent& event)
{
    if (rowSelected)
    {
        if (wxMessageBox("Are you sure you want to delete the selected row?",
            "Confirm action",
            wxICON_QUESTION | wxYES_NO) != wxYES)
        {
            event.Skip();
            return;
        }

        wxDataViewListCtrl* table = tables.at(selectedTable);

        // Getting table selection 
        wxDataViewItem item = table->GetSelection();

        // Getting the data model
        wxDataViewModel* model = table->GetModel();

        wxVariant columnValue;
        model->GetValue(columnValue, item, 0);

        // Getting selected row id
        wxString selectedRowId = columnValue.GetString();

        // Gettind the name of id column
        wxString IdColumnName = table->GetColumn(0)->GetTitle();

        try
        {
            sqlController->ExecuteSQL(("DELETE FROM "s + selectedTable + " WHERE " + IdColumnName + " = " + selectedRowId).ToUTF8());
        }
        catch (Exception& exp)
        {
            wxLogError(exp.what().c_str());
        }
        UpdateTable(selectedTable);

        updateDataButton->Disable();
        deleteDataButton->Disable();

        rowSelected = false;
    }
}

void GUI::OnTotalInfoButtonClicked(wxCommandEvent& event)
{
    TotalInfoDialog* totalInfoDialog = new TotalInfoDialog(panel, sqlController);
    totalInfoDialog->ShowModal();
}

void GUI::OnSelectionChanged(wxDataViewEvent& event)
{
    wxDataViewListCtrl* table = tables.at(selectedTable);

    // Check if any row is highlighted in data list view
    wxDataViewItem item = table->GetSelection();
    if (item.IsOk())
    {
        rowSelected = true;
        updateDataButton->Enable();
        deleteDataButton->Enable();
    }
    else
    {
        rowSelected = false;
        updateDataButton->Disable();
        deleteDataButton->Disable();
    }
    
}

AddDataDialog::AddDataDialog(wxWindow* parent, SQLController* sqlController, wxString selectedTable, wxDataViewListCtrl* table)
    : wxDialog(parent, wxID_ANY, "Add data")
{
    this->sqlController = sqlController;

    this->selectedTable = selectedTable;

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Getting the number of columns
    unsigned int columnCount = table->GetColumnCount();

    // Getting column names and adding to the form
    for (int i = 0; i < columnCount; ++i) {
        // Create data input controls
        wxStaticText* columnName = new wxStaticText(this, wxID_ANY, table->GetColumn(i)->GetTitle());
        wxTextCtrl* textCtrl = new wxTextCtrl(this, wxID_ANY);

        sizer->Add(columnName, 0, wxALL, 5);
        sizer->Add(textCtrl, 0, wxALL | wxEXPAND, 5);

        formFields.push_back(textCtrl);
    }

    // Button to save data
    wxButton* saveButton = new wxButton(this, wxID_OK, "Save");
    saveButton->Bind(wxEVT_BUTTON, &AddDataDialog::OnSave, this);

    // Button to cancel
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    buttonSizer->Add(saveButton, 0, wxALL | wxEXPAND, 5);
    buttonSizer->Add(cancelButton, 0, wxALL | wxEXPAND, 5);

    // Placing controls using boxSizer
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
    
    // Creating statement like value1, value2..."
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        const char* type = (char*)sqlite3_column_text(stmt, 0);

        if (std::strcmp(type, "VARCHAR(255)") == 0 || std::strcmp(type, "TEXT") == 0)
        {
            values += "\""s + formFields[i]->GetValue() + "\"";
        }
        else
        {
            values += formFields[i]->GetValue();
            if (values.compare(""))
            {
                values += "0";
            }
        }

        if (i < totalRows)
        {
            values += ", ";

        }
        i++;
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt);

    try 
    {
       sqlController->ExecuteSQL(("INSERT INTO "s + selectedTable + " VALUES(" + values + ")").ToUTF8());

       // Close the window after saving
       EndModal(wxID_OK);

       wxMessageBox("Data added successfully");
    }
    catch (Exception& exp)
    {
        wxLogError(exp.what().c_str());
    }
    
}

UpdateDataDialog::UpdateDataDialog(wxWindow* parent, SQLController* sqlController, wxString selectedTable, wxDataViewListCtrl* table)
    : wxDialog(parent, wxID_ANY, "Update data")
{
    this->sqlController = sqlController;

    this->selectedTable = selectedTable;

    this->table = table;

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Getting table selection 
    wxDataViewItem item = table->GetSelection();

    // Getting the data model
    wxDataViewModel* model = table->GetModel();

    // Getting the number of columns
    unsigned int columnCount = table->GetColumnCount();

    wxVariant columnValue;
    model->GetValue(columnValue, item, 0);

    // Getting selected row id
    this->selectedRowId = columnValue.GetString();

    // Gettind the name of id column
    this->IdColumnName = table->GetColumn(0)->GetTitle();

    // Getting column names and adding to the form
    for (int i = 0; i < columnCount; ++i) {
        // Create data input controls
        wxStaticText* columnName = new wxStaticText(this, wxID_ANY, table->GetColumn(i)->GetTitle());

        // Get the value in the columns of the selected row
        model->GetValue(columnValue, item, i);

        wxTextCtrl* textCtrl = new wxTextCtrl(this, wxID_ANY, columnValue.GetString());

        if (i == 0)
        {
            textCtrl->SetEditable(false);
        }

        sizer->Add(columnName, 0, wxALL, 5);
        sizer->Add(textCtrl, 0, wxALL | wxEXPAND, 5);

        formFields.push_back(textCtrl);
    }

    // Button to save data
    wxButton* saveButton = new wxButton(this, wxID_OK, "Save");
    saveButton->Bind(wxEVT_BUTTON, &UpdateDataDialog::OnSave, this);

    // Button to cancel
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    buttonSizer->Add(saveButton, 0, wxALL | wxEXPAND, 5);
    buttonSizer->Add(cancelButton, 0, wxALL | wxEXPAND, 5);

    // Placing controls using wxSizer
    sizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTER, 5);

    SetSizer(sizer);
    sizer->Fit(this);
    sizer->SetSizeHints(this);
}

void UpdateDataDialog::OnSave(wxCommandEvent& event)
{
    // Getting column types from the table
    sqlite3_stmt* stmt = sqlController->PrepareSQL(("SELECT type FROM pragma_table_info(\'"s + selectedTable + "\')").c_str());

    std::string values = "";

    int totalRows = formFields.size() - 1;

    int i = 0;

    // Creating statement like "columnName1 = value1, columnName2 = value2..."
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        const char* type = (char*)sqlite3_column_text(stmt, 0);

        values += table->GetColumn(i)->GetTitle() + " = ";

        if (std::strcmp(type, "VARCHAR(255)") == 0 || std::strcmp(type, "TEXT") == 0)
        {
            values += "\""s + formFields[i]->GetValue() + "\"";
        }
        else
        {
            values += formFields[i]->GetValue();
            if (values.compare(""))
            {
                values += "0";
            }
        }

        if (i < totalRows)
        {
            values += ", ";
        }
        i++;
    }

    // Release resources associated with a prepared stmt SQL query
    sqlite3_finalize(stmt);

    try
    {
        sqlController->ExecuteSQL(("UPDATE "s + selectedTable + " SET " + values + " WHERE " + IdColumnName + " = " + selectedRowId).ToUTF8());

        // Close the window after saving
        EndModal(wxID_OK);

        wxMessageBox("Data updated successfully");
    }
    catch (Exception& exp)
    {
        wxLogError(exp.what().c_str());
    }
}

AboutProgramDialog::AboutProgramDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "About program")
{
    wxNotebook* notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(825, 400), wxNB_FIXEDWIDTH);

    // Creating the first tab
    wxPanel* aboutProgramPanel = new wxPanel(notebook);

    wxBoxSizer* vSizer1 = new wxBoxSizer(wxVERTICAL);

    wxStaticText* headline = new wxStaticText(aboutProgramPanel, wxID_ANY, "Household Appliances Store");
    wxFont font = headline->GetFont();
    font.MakeBold();
    font.SetPointSize(15);
    headline->SetFont(font);

    vSizer1->AddStretchSpacer();
    vSizer1->Add(headline, 0, wxALIGN_CENTRE | wxALL, 5);
    vSizer1->Add(new wxStaticText(aboutProgramPanel, wxID_ANY, "A software product for working with a file database of household appliances store products"), 0, wxALIGN_CENTRE | wxALL, 5);
    vSizer1->Add(new wxStaticText(aboutProgramPanel, wxID_ANY, "Portable version"), 0, wxALIGN_CENTRE | wxALL, 10);
    vSizer1->Add(new wxStaticText(aboutProgramPanel, wxID_ANY, "Author and developer:"), 0, wxALIGN_CENTRE | wxALL, 5);
    vSizer1->Add(new wxStaticText(aboutProgramPanel, wxID_ANY, "Borysenko Rostyslav Vitaliyovych, HNEU student"), 0, wxALIGN_CENTRE | wxALL, 5);
    vSizer1->AddStretchSpacer();

    aboutProgramPanel->SetSizer(vSizer1);

    // Adding the first tab to Notepad
    notebook->AddPage(aboutProgramPanel, "About program");

    // Creating the second tab
    wxPanel* aboutFunctionalPanel = new wxPanel(notebook);
    
    wxBoxSizer* hSizer2 = new wxBoxSizer(wxHORIZONTAL);

    wxInitAllImageHandlers();

    // Loading add data image
    wxImage addDataImage(wxT("img\\add_data.png"), wxBITMAP_TYPE_PNG);
    wxBitmap* bitmap = new wxBitmap(addDataImage);

    // Creating a static element to display the image
    wxStaticBitmap* addDataImageBitmap = new wxStaticBitmap(aboutFunctionalPanel, wxID_ANY, *bitmap);

    wxBoxSizer* addDataSizer = new wxBoxSizer(wxVERTICAL);

    addDataSizer->Add(addDataImageBitmap, 0, wxALL | wxALIGN_CENTER);

    wxStaticText* addDataHeadline = new wxStaticText(aboutFunctionalPanel, wxID_ANY, "Add data(Ctrl-A)");

    wxFont headlineFont = addDataHeadline->GetFont();
    headlineFont.MakeBold();
    headlineFont.SetPointSize(14);
    addDataHeadline->SetFont(headlineFont);
    
    addDataSizer->Add(addDataHeadline, 0, wxALL | wxALIGN_CENTER, 5);

    addDataSizer->Add(new wxStaticText(aboutFunctionalPanel, wxID_ANY, "The method of adding data works as follows:\nClick on the \"Add\" button or the \"Ctrl-A\" key\ncombination. A dialog box opens with a form\nfor inserting a row into the table. Click the \"Save\"\nbutton to save the data.\n\nNote: Unique columns must be inscribed."), 0, wxALL | wxALIGN_LEFT, 5);

    // Loading update data image
    wxImage updateDataImage(wxT("img\\update_data.png"), wxBITMAP_TYPE_PNG);
    bitmap = new wxBitmap(updateDataImage);

    // Creating a static element to display the image
    wxStaticBitmap* updateDataImageBitmap = new wxStaticBitmap(aboutFunctionalPanel, wxID_ANY, *bitmap);
    
    wxBoxSizer* updateDataSizer = new wxBoxSizer(wxVERTICAL);

    updateDataSizer->Add(updateDataImageBitmap, 0, wxALL | wxALIGN_CENTER);

    wxStaticText* updateDataHeadline = new wxStaticText(aboutFunctionalPanel, wxID_ANY, "Update data(Ctrl-U)");

    updateDataHeadline->SetFont(headlineFont);

    updateDataSizer->Add(updateDataHeadline, 0, wxALL | wxALIGN_CENTER, 5);

    updateDataSizer->Add(new wxStaticText(aboutFunctionalPanel, wxID_ANY, "The data update method works as follows:\nSelect the row in which you want to update\nthe data. Click the \"Update\" button or press\nthe \"Ctrl-U\" key combination. A dialog box\nwith the data update form opens. Click on\nthe \"Save\" button to save the data.\n\nNote: Unique columns must always be entered."), 0, wxALL | wxALIGN_LEFT, 5);

    // Loading delete data image
    wxImage deleteDataImage(wxT("img\\delete_data.png"), wxBITMAP_TYPE_PNG);
    bitmap = new wxBitmap(deleteDataImage);

    // Creating a static element to display the image
    wxStaticBitmap* deleteDataImageBitmap = new wxStaticBitmap(aboutFunctionalPanel, wxID_ANY, *bitmap);

    wxBoxSizer* deleteDataSizer = new wxBoxSizer(wxVERTICAL);

    deleteDataSizer->Add(deleteDataImageBitmap, 0, wxALL | wxALIGN_CENTER);

    wxStaticText* deleteDataHeadline = new wxStaticText(aboutFunctionalPanel, wxID_ANY, "Delete data(Del)");

    deleteDataHeadline->SetFont(headlineFont);

    deleteDataSizer->Add(deleteDataHeadline, 0, wxALL | wxALIGN_CENTER, 5);

    deleteDataSizer->Add(new wxStaticText(aboutFunctionalPanel, wxID_ANY, "The method for deleting data works as follows:\nSelect the row in which you want to delete data.\nPress the \"Delete\" button or the \"Del\" key.\nConfirm the action in the dialog box."), 0, wxALL | wxALIGN_LEFT, 5);

    hSizer2->Add(addDataSizer, 0, wxALL | wxEXPAND, 5);

    hSizer2->Add(updateDataSizer, 0, wxALL | wxEXPAND, 5);

    hSizer2->Add(deleteDataSizer, 0, wxALL | wxEXPAND, 5);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->AddStretchSpacer();

    mainSizer->Add(hSizer2, 0, wxALL | wxEXPAND, 0);

    mainSizer->AddStretchSpacer();
    
    aboutFunctionalPanel->SetSizer(mainSizer);

    // Adding the second tab to Notepad
    notebook->AddPage(aboutFunctionalPanel, "Functional");

    // Creating close button 
    wxButton* closeButton = new wxButton(this, wxID_CLOSE, "Close", wxDefaultPosition, wxSize(95, 25));
    closeButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) { EndModal(wxID_CLOSE); });

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(notebook, 0, wxALL | wxEXPAND, 10);
    sizer->Add(closeButton, 0, wxALL | wxALIGN_RIGHT, 10);
    
    SetSizer(sizer);
    sizer->Fit(this);
    sizer->SetSizeHints(this);
}

AboutDeveloperDialog::AboutDeveloperDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "About developer")
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);

    static wxStaticText* text = new wxStaticText(this, wxID_ANY, "Developer: Borysenko Rostyslav Vitaliyovych");

    wxFont font = text->GetFont();
    font.SetPointSize(17);
    text->SetFont(font);

    vSizer->Add(text, 0, wxALL | wxEXPAND, 10);

    text = new wxStaticText(this, wxID_ANY, "Employment: Student");
    text->SetFont(font);
    vSizer->Add(text, 0, wxALL | wxEXPAND, 10);

    text = new wxStaticText(this, wxID_ANY, "Place of study: HNEU");
    text->SetFont(font);
    vSizer->Add(text, 0, wxALL | wxEXPAND, 10);

    text = new wxStaticText(this, wxID_ANY, "E-mail: rostyslavborysenko04@gmail.com");
    text->SetFont(font);
    vSizer->Add(text, 0, wxALL | wxEXPAND, 10);

    wxInitAllImageHandlers();

    // Loading the developer's image
    wxImage image(wxT("img\\developer.jpg"), wxBITMAP_TYPE_JPEG);
    image.Rescale(246, 300, wxIMAGE_QUALITY_HIGH);
    wxBitmap bitmap(image);

    // Creating a static element to display the image
    wxStaticBitmap* developerImage = new wxStaticBitmap(this, wxID_ANY, bitmap);

    // Creating close button 
    wxButton* closeButton = new wxButton(this, wxID_CLOSE, "Close", wxDefaultPosition, wxSize(95, 25));
    closeButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) { EndModal(wxID_CLOSE); });

    hSizer->Add(vSizer, 0, wxALL | wxCENTER, 5);
    hSizer->Add(developerImage, 0, wxALL | wxEXPAND, 50);

    sizer->Add(hSizer, 0, wxALL | wxCENTER, 0);
    sizer->Add(closeButton, 0, wxALL | wxALIGN_RIGHT, 10);

    SetSizer(sizer);
    sizer->Fit(this);
    sizer->SetSizeHints(this);
}

TotalInfoDialog::TotalInfoDialog(wxWindow* parent, SQLController* sqlController)
    : wxDialog(parent, wxID_ANY, "Total information", wxDefaultPosition, wxSize(950, 500))
{

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxPanel* panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, GetSize());

    wxDataViewListCtrl* table = new wxDataViewListCtrl(panel, wxID_ANY);

    // Getting data from database
    sqlite3_stmt* stmt = sqlController->PrepareSQL(R"(
        SELECT
            p.ProductID,
            p.ProductName,
            p.Description,
            p.Price,
            p.StockQuantity,
            pc.CategoryName,
            pc.AverageRating AS AverageCategoryRating,
            m.Name AS ManufacturerName,
            m.CountryOfOrigin,
            m.WebsiteURL
        FROM
            Products p
        JOIN
            ProductCategories pc ON p.CategoryID = pc.CategoryID
        JOIN
            Manufacturers m ON p.ManufacturerID = m.ManufacturerID;
    )"
    );

    // Adding column names to a table
    for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
        wxString columnName = wxString::FromUTF8((char*)sqlite3_column_name(stmt, i));
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

    table->SetBackgroundColour(wxColor(31, 31, 31, 255));
    table->SetAlternateRowColour(wxColor(31, 31, 31, 255));
    table->SetForegroundColour(wxColor(*wxWHITE));

    // Creating close button 
    wxButton* closeButton = new wxButton(panel, wxID_CLOSE, "Close", wxDefaultPosition, wxSize(95, 25));
    closeButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) { EndModal(wxID_CLOSE); });

    // Add the table and close button to the vertical box-sizer
    sizer->Add(table, 1, wxEXPAND | wxALL, 10);
    sizer->Add(closeButton, 0, wxALL | wxALIGN_RIGHT, 10);

    panel->SetSizer(sizer);

    panel->Layout();

    sqlite3_finalize(stmt);
}
