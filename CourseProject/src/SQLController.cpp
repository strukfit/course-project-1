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

void SQLController::OpenDB(const char* filename)
{
    if (sqlite3_open(filename, &db))
    {
        throw(Exception("Can't open database: "s + sqlite3_errmsg(db)));
    }
}

void SQLController::ExecuteSQL(const char* sql)
{
    sqlite3_stmt* stmt;

    char* errMsg = 0;

    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {

        throw(Exception("Error when executing SQL query: "s + errMsg));
        sqlite3_free(errMsg);
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
        CREATE TABLE IF NOT EXISTS Manufacturers (
            ManufacturerID INTEGER PRIMARY KEY AUTOINCREMENT,
            Name VARCHAR(255) UNIQUE,
            CountryOfOrigin VARCHAR(255),
            ContactInfo TEXT
        );

        CREATE TABLE IF NOT EXISTS Products(
            ProductID INTEGER PRIMARY KEY AUTOINCREMENT,
            ProductName VARCHAR(255) UNIQUE,
            Description TEXT,
            Price DECIMAL(10, 2),
            StockQuantity INT,
            CategoryID INT,
            ManufacturerID INT,
            FOREIGN KEY(CategoryID) REFERENCES ProductCategories(CategoryID),
            FOREIGN KEY(ManufacturerID) REFERENCES Manufacturers(ManufacturerID)
        );

        CREATE TABLE IF NOT EXISTS ProductCategories (
            CategoryID INTEGER PRIMARY KEY AUTOINCREMENT,
            CategoryName VARCHAR(255) UNIQUE,
            Description TEXT
        );
        
        )"
    );

    Test();

    /*SQLController sqlController("database.db");

    if (sqlController.executeSQL("CREATE TABLE IF NOT EXISTS MyTable (ID INTEGER PRIMARY KEY, Name TEXT)") == SQLITE_OK) {
        wxLogMessage("Таблица создана или уже существует.");
    }*/
}

sqlite3_stmt* SQLController::SelectData(const char* tableName)
{
    std::string selectData = "SELECT * FROM "s + tableName;
    
    return PrepareSQL(selectData.c_str());
}

void SQLController::Test()
{
    ExecuteSQL(R"(
        INSERT OR IGNORE INTO Manufacturers (Name, CountryOfOrigin, ContactInfo)
        VALUES
            ('Samsung', 'South Korea', 'www.samsung.com/contact'),
            ('LG', 'South Korea', 'www.lg.com/contact'),
            ('Whirlpool', 'United States', 'www.whirlpool.com/contact'),
            ('Sony', 'Japan', 'www.sony.com/contact');

        INSERT OR IGNORE INTO ProductCategories (CategoryName, Description)
        VALUES
            ('Refrigerators', 'Appliances used for cooling and preserving food'),
            ('Washing Machines', 'Appliances for laundry purposes'),
            ('Televisions', 'Electronics for entertainment'),
            ('Microwaves', 'Kitchen appliances for quick cooking');

        INSERT OR IGNORE INTO Products (ProductName, Description, Price, StockQuantity, CategoryID, ManufacturerID)
        VALUES
            ('Samsung Refrigerator', 'Double door refrigerator with freezer', 799.99, 20, 1, 1),
            ('LG Washing Machine', 'Front-loading washing machine', 499.99, 15, 2, 2),
            ('Sony 55" Smart TV', '4K Ultra HD LED TV with Smart features', 799.99, 10, 3, 4),
            ('Whirlpool Microwave', '1.2 cu. ft. countertop microwave', 149.99, 25, 4, 3);

    )"
    );
}
