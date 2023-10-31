#include "SQLController.h"

SQLController::SQLController() : 
    db(nullptr),
    dbOpeningError(("Can't open database: %s\n", sqlite3_errmsg(db)))
{

}

SQLController::~SQLController()
{
    if (db) 
    {
        sqlite3_close(db);
    }
}

int SQLController::executeSQL(const char* sql)
{
    char* errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "Error when executing SQL query: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    return rc;
}

void SQLController::openDB(const char* filename)
{
    
    int rc = sqlite3_open(filename, &db);
    
    if (rc)
    {
        throw(dbOpeningError);
    }
}

