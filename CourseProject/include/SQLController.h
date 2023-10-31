#pragma once

#include <sqlite3.h>
#include "Exception.h"

class SQLController
{
    sqlite3* db; // Database

public:
    SQLController();

    ~SQLController();

    /// <summary>
    /// The function executes an SQL query
    /// </summary>
    /// <param name="sql - SQL statement"></param>
    /// <returns></returns>
    int executeSQL(const char* sql);

    /// <summary>
    /// Creates and opens database
    /// </summary>
    /// <param name="name - database name"></param>
    void openDB(const char* name);

    /*void createData();

    void deleteData(); 

    void updateData();

    void selectData();*/

    Exception dbOpeningError;
};