#pragma once

#include <sqlite3.h>
#include "Exception.h"

class SQLController
{
    sqlite3* db; // Database
    
    bool dbIsEmptyFlag;

    Exception dbOpeningError;
    Exception sqlExecutionError;

public:
    SQLController();

    ~SQLController();

    void setDbIsEmptyFlag(bool flag);

    bool dbIsEmpty();

    /// <summary>
    /// Creates and opens database
    /// </summary>
    /// <param name="name - database name"></param>
    void openDB(const char* name);

    /// <summary>
    /// The function executes an SQL query
    /// </summary>
    /// <param name="sql - SQL statement"></param>
    /// <returns></returns>
    void executeSQL(const char* sql);

    /// <summary>
    /// 
    /// </summary>
    void databaseInit();

    void createData();

    void deleteData(); 

    void updateData();

    void selectData();

    void Test();
};