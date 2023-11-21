#include "SQLController.h"

using namespace std::string_literals;

SQLController::SQLController() :
    db(nullptr)
{

}

SQLController::~SQLController()
{
    if (db) 
    {
        sqlite3_close(db);
    }
}

bool SQLController::isDatabaseExist(const std::string& dbName)
{
    std::ifstream file(dbName);
    return file.good();
}

void SQLController::OpenDB(const char* filename)
{
    if (sqlite3_open(filename, &db))
    {
        throw(Exception("Error in opening database: "s + sqlite3_errmsg(db)));
    }
}

void SQLController::ExecuteSQL(const char* sql)
{
    if (sqlite3_exec(db, sql, 0, 0, nullptr) != SQLITE_OK)
    {   
        int errorCode = sqlite3_errcode(db);
        std::string sqliteError = sqlite3_errmsg(db);
        std::string columnName = "";
        std::string error;

        if (errorCode == 19 && sqliteError != "FOREIGN KEY constraint failed")
        {
            // Find the last occurrence of a space in the sqlite error
            size_t lastSpace = sqliteError.find_last_of(' ');

            // Equate error to the substring after the last space
            columnName = sqliteError.substr(lastSpace + 1);

            error = "The column " + columnName + " with this value already exists.";
        }
        else if (errorCode == 1)
        {
            error = "Some column has a different data type.";
        }
        else
        {
            error = sqliteError;
        }
        
        throw(Exception("Error when executing SQL query: "s + error));
    }
}

sqlite3_stmt* SQLController::PrepareSQL(const char* sql)
{
    sqlite3_stmt* stmt;
    // Preparing SQL statement(compilation into a byte-code program) 
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        throw(Exception("Error in preparation of SQL query: "s + sqlite3_errmsg(db)));
    }

    return stmt;
}

void SQLController::DatabaseInit()
{
    ExecuteSQL(R"(
        PRAGMA foreign_keys = ON;

        CREATE TABLE IF NOT EXISTS Products (
            ProductID INTEGER PRIMARY KEY AUTOINCREMENT,
            ProductName TEXT NOT NULL UNIQUE,
            Description TEXT,
            Price REAL,
            StockQuantity INTEGER,
            CategoryID INTEGER,
            ManufacturerID INTEGER,
            FOREIGN KEY (CategoryID) REFERENCES ProductCategories(CategoryID) ON DELETE RESTRICT,
            FOREIGN KEY (ManufacturerID) REFERENCES Manufacturers(ManufacturerID) ON DELETE RESTRICT
        );

        CREATE TABLE IF NOT EXISTS ProductCategories (
            CategoryID INTEGER PRIMARY KEY AUTOINCREMENT,
            CategoryName TEXT NOT NULL UNIQUE,
            AverageRating REAL,
            Description TEXT
        );

        CREATE TABLE IF NOT EXISTS Manufacturers (
            ManufacturerID INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL UNIQUE,
            CountryOfOrigin TEXT,
            ContactInfo TEXT,
            WebsiteURL TEXT,
            ProductionFacilities TEXT,
            Certifications TEXT
        );        
        )"
    );
}

sqlite3_stmt* SQLController::SelectData(const char* tableName)
{
    std::string selectData = "SELECT * FROM "s + tableName;
    
    return PrepareSQL(selectData.c_str());
}