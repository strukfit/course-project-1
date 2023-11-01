#include "GUI.h"

wxPanel* GUI::panel;

void GUI::MainFrame(wxFrame* mainFrame)
{
	mainFrame->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& event)
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

	panel = new wxPanel(mainFrame, wxID_ANY, wxDefaultPosition, wxSize(800, 600));
	panel->SetBackgroundColour(wxColour(43, 43, 43, 255));
	/*wxButton* button = new wxButton(panel, wxID_ANY, "Create DB", wxPoint(0, 0), wxSize(200, 50));
	button->SetBackgroundColour(wxColor(55, 55, 55, 255));
	button->SetForegroundColour(wxColor(*wxWHITE));

	button->Bind(wxEVT_BUTTON, &GUI::OnButtonClicked);*/
    mainFrame->CreateStatusBar();
	mainFrame->Show();
}

void GUI::ErrorFrame(const char* errorName)
{
	//wxMessageDialog dlg(mainFrame, errorName);
}

void GUI::ShowTable(SQLController sqlController, const char* tableName)
{
    wxListCtrl* table = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    sqlite3_stmt* stmt = sqlController.SelectData(tableName);
    
    // Adding column names to a table
    for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
        table->AppendColumn((char*)sqlite3_column_name(stmt, i));
    }

    // Populating a table with database data
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        long index = table->InsertItem(table->GetItemCount(), wxString::Format("%d", id));
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            const char* data = (char*)sqlite3_column_text(stmt, i);
            table->SetItem(index, i, data);
        }
    }
    table->SetBackgroundColour(wxColor(31, 31, 31, 255));
    table->SetTextColour(wxColor(255, 255, 255, 255));

    sqlite3_finalize(stmt);
}

void GUI::OnButtonClicked(wxCommandEvent& event)
{
	
}
