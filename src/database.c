#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sqlite3.h>
#include "database.h"
#include "helper.c"
#include "config.c"

#define SIZE 1024

/**
 *  Database module charged to implement hierarchy of categories
 **/

/* Show informations with the callback */
static int show = 0;

char *getDatabase(){
    char *path = malloc(strlen(ROOT) + strlen(DATABASE)+1);
    strcpy(path, ROOT);
    strcat(path, DATABASE);
    strcat(path, "\0");
    return path;
}

/* Injection of variables for SQL requestes */
char *injection(char const *part1, char const *category, char const *part2)
{
    char *result = malloc(strlen(part1) + strlen(category) + strlen(part2) + 1);
    strcpy(result, part1);
    strcat(result, category);
    strcat(result, part2);
    strcat(result, "\0");
    return result;
}

/* Display errors from SQL */
void check(int rc, char *errors)
{
    if (rc != SQLITE_OK)
    {
        if(show==1)
            fprintf(stderr, "SQL error: %s\n", errors);
        sqlite3_free(errors);
    }
}

/* Callback for display tree of hierarchy*/
int callback(void *data, int argc, char **argv, char **column)
{   
    if(show==0)
        return 0;
    for (int i = 0; i < argc; i++)
    {
        if(i%4==0){
            char level[10];
            for (size_t j = 0; j < atoi(argv[2]); j++)
            {
                strcat(level, ".....");
            }
            strcat(level, "\0");
            
            printf("%s%s\n", level, argv[3] ? argv[0] : "NULL");
        }
    }
    return 0;
}

/*  
*   Init database file, load tables from .sql file   
*   Create .db file where data is stored.
*/
void init_db()
{
    sqlite3 *db;
    char *errors = 0;
    char *path_database = getDatabase();
    int rc = sqlite3_open(path_database, &db);
    free(path_database);
    if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return;
    } else {
      fprintf(stderr, "Opened database successfully\n");
    }
    char *table = readString(SQL_FILE);
    rc = sqlite3_exec(db, table, callback, 0, &errors);
    free(table);
    check(rc, errors);
    sqlite3_close(db);
}

/**
 *  Select a row from category name, and return id 
 **/
int select_db(char const *category)
{
    sqlite3 *db;
    sqlite3_stmt *res;
    char *errors = 0;
    char *path_database = getDatabase();
    int rc = sqlite3_open(path_database, &db);
    free(path_database);
    check(rc, errors);
    char *sql = injection("SELECT * FROM relations WHERE category='", category,"' LIMIT 1;");
    int id = 0;
    rc = sqlite3_prepare_v2(db, sql, 128, &res, NULL);
    if (sqlite3_step(res) == SQLITE_ROW)
    {
        id = sqlite3_column_int(res, 0);
    }
    free(sql);
    check(rc, errors);
    sqlite3_finalize(res);
    sqlite3_close(db);
    return id;
}

/**
 *  Insert category in db. 
 *  Root category need to be inserted as follow: insert_db("_", "category")
 *  Next: insert_db("category", "sub_category") -> again and again 
 **/
int insert_db(char const *parent_category, char const *child_category)
{
    int parent = select_db(parent_category);
    if (parent == 0 && strstr(parent_category, "_") == NULL)
    {
        printf("Tag %s does't exist!\n", parent_category);
        return EXIT_FAILURE;
    }
    sqlite3 *db;
    char *path_database = getDatabase();
    int rc = sqlite3_open(path_database, &db);
    free(path_database);
    char *errors = 0;
    char char_id[128];
    sprintf(char_id, "%i", parent);
    char *sql0 = injection("', ",char_id,");");
    char *sql1 = injection("INSERT INTO relations (category,parent) VALUES ('", child_category, sql0);
    rc = sqlite3_exec(db, sql1, callback, 0, &errors);
    free(sql0);
    free(sql1);
    check(rc, errors);
    sqlite3_close(db);
    return 1;
}

/**
 *  Delete on cascade a category from db
 *  That means than: when a category is deleted -> all those children are deleted too
 *  !!! Foreign key is not working in this SQLite version !!! 
 **/
int delete_db(char const *category)
{
    sqlite3 *db;
    char *errors = 0;
    char *path_database = getDatabase();
    int rc = sqlite3_open(path_database, &db);
    free(path_database);
    char sql[SIZE] = "DELETE FROM relations WHERE category=''";
    char *parents = from_parent_to_children(category);
    char *token = strtok(parents, " ");
    while (token != NULL)
    {
        strcat(sql, "OR category=");
        strcat(sql, "'");
        strcat(sql, token);
        strcat(sql, "'");
        token = strtok(NULL, " ");
    }
    strcat(sql, "\0");
    rc = sqlite3_exec(db, sql, callback, 0, &errors);
    check(rc, errors);
    free(parents);
    sqlite3_close(db);
    return 0;
}

/**
 * Execute a SQL query 
 **/
char *query(char const *part1, char const *category, char const *part2){
    char *sql = injection(part1, category, part2);
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *errors = 0;
    char *path_database = getDatabase();
    int rc = sqlite3_open(path_database, &db);
    free(path_database);
    char *values = malloc(SIZE);
    strcpy(values, "");
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {      
        strcat(values, (char *)sqlite3_column_text(stmt, 0));
        strcat(values, " ");
    }
    rc = sqlite3_exec(db, sql, callback, 0, &errors);
    sqlite3_finalize(stmt);
    strcat(values, "\0");
    check(rc, errors);
    free(sql);
    return values;
}

/**
 *  Get path from child to parent 
 **/
char *from_child_to_parent(char const *category)
{
    return query("WITH tree (data, id) AS (SELECT category, parent FROM relations WHERE category='", category, "' UNION ALL SELECT category, parent FROM relations r INNER JOIN tree t ON t.id=r.id) SELECT * FROM tree;");
}

/**
 *  Get path from parent to all children 
 **/
char *from_parent_to_children(char const *category)
{   
    return query("WITH tree (data, id, level, str) AS (SELECT category, id, 0, CAST('' AS NVARCHAR(256)) FROM relations r WHERE category='", category, "' UNION ALL SELECT category, r.id, t.level+1, t.str||category FROM relations r INNER JOIN tree t ON t.id = r.parent) SELECT * FROM tree t GROUP BY str, id");
}

/**
 *  Print tree of a category
 **/
void printTree(char const *category)
{   
    show = 1;
    char *tree = from_parent_to_children(category);
    free(tree);
    show = 0;
}

/**
 *  Print all avaible Root categories 
 **/
void avaible()
{
    char const *result = query("SELECT category FROM relations WHERE parent=0", "","");
    printf("Hierarchy avaible for the following tags: %s!\n", strlen(result)>1 ? result : "no one");
}