#pragma once

#include <sqlite3.h>
#include "Exception.h"

class SQLController
{
    sqlite3* db; // Database

    Exception dbOpeningError;
    Exception sqlExecutionError;
    Exception sqlPreparationError;

public:
    SQLController();

    ~SQLController();

    /// <summary>
    /// Creates and opens database
    /// </summary>
    /// <param name="name - database name"></param>
    void OpenDB(const char* name);

    /// <summary>
    /// The function executes an SQL query
    /// </summary>
    /// <param name="sql - SQL statement"></param>
    /// <returns></returns>
    void ExecuteSQL(const char* sql);

    /// <summary>
    /// Database initialization
    /// </summary>
    void DatabaseInit();

    void CreateData();

    void DeleteData(); 

    void UpdateData();

    sqlite3_stmt* SelectData(const char* tableName);

    void Test();
};