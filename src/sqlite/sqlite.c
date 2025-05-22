#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include "sqlite.h"

ULONGLONG insert_data(sqlite3 *db, sqlite3_stmt *stmt, char *path, ULONGLONG size, ULONGLONG parent_folder_id)
{
    sqlite3_bind_text(stmt, 1, path, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, size);
    sqlite3_bind_int(stmt, 3, parent_folder_id);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    ULONGLONG id = (ULONGLONG)sqlite3_last_insert_rowid(db);

    return id;
}

int save_graph_elemnts(sqlite3 *db, sqlite3_stmt *stmt, Node *node, ULONGLONG parent_folder_id)
{
    ULONGLONG id = insert_data(db, stmt, node->path, node->size, parent_folder_id);
    if (id < 1)
    {
        return 1;
    }
    for (int i = 0; i < node->childNum; i++)
    {
        if (save_graph_elemnts(db, stmt, node->children[i], id) != 0)
        {
            return 1;
        }
    }
    return 0;
}

int save_graph(Node *node)
{
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open("./build/files.db", &db);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database");
        return 1;
    }

    const char *drops_files_table_sql = "DROP TABLE IF EXISTS files;";
    const char *create_files_table_sql =
        "CREATE TABLE files ("
        "id INTEGER PRIMARY KEY, "
        "path TEXT, "
        "size TEXT, "
        "parent_folder_id INTEGER"
        ");";

    rc = sqlite3_exec(db, drops_files_table_sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_exec(db, create_files_table_sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_stmt *stmt;
    const char *prepare_insert = "INSERT INTO files(path, size, parent_folder_id) VALUES(?, ?, ?);";

    rc = sqlite3_prepare_v2(db, prepare_insert, -1, &stmt, NULL);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    if (save_graph_elemnts(db, stmt, node, 0) != 0)
    {
        sqlite3_close(db);
        return 1;
    }
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);

    sqlite3_finalize(stmt);

    const char *sql_drop_index = "DROP INDEX IF EXISTS idx_files;";
    const char *sql_create_index = "CREATE INDEX idx_files ON files(parent_folder_id);";
    rc = sqlite3_exec(db, sql_drop_index, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to drop index: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_exec(db, sql_create_index, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to create index: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_close(db);
    return 0;
}