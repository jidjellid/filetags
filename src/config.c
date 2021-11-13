#ifndef __CONFIG__C__
#define __CONFIG__C__

#include "helper.c"

#define GLOBALLY 1

#if GLOBALLY == 1
    #define ROOT getenv("HOME")
    #define DATABASE "/.tags/tags.db"
    #define DOCS "/.tags/tag.txt"
#else
    #define ROOT ""
    #define DATABASE "./database/tags.db" 
    #define DOCS "./docs/tag.txt"
#endif

/* Extended attribute is linked with the username, tag system in not shared between users */
#define ATTR "user.tags."
#define SQL_FILE "./database/tags.sql"

#endif