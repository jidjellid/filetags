#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.c"
#include "tag.c"

#define ADD 0
#define DELETE 1
#define GET 2
#define SEARCH 3
#define ADDGRAD 4
#define DELETEGRAD 5
#define TREE 6
#define HELP 7
#define TRUNCATE 8
#define TOTAL_CMD 9

char const *options[] = {"add", "delete", "get", "search", "addgrad", "deletegrad", "tree", "help", "truncate"};

/* Link #define with an id */
int getOption(char const *option){
    for (int i = 0; i < TOTAL_CMD; i++)
    {
        int is_cmd = strcmp(option, options[i]);
        if(is_cmd==0)
        {
            return i;
        }
    }
    return -1;
}

int main(int argc, char const *argv[])
{   
    if (argc > 1)
    {   
        int option = getOption(argv[1]);
        switch (option)
        {
            case ADD:
                add(argc, argv);
                break;
            case DELETE:
                delete(argc, argv);
                break;
            case GET:
                get(argc, argv);
                break;
            case SEARCH:
                search(argc, argv);
                break;
            case ADDGRAD:
                addgrad(argc, argv);
                break;
            case DELETEGRAD:
                deletegrad(argc, argv);
                break;
            case TREE:
                tree(argc, argv);
                break;
            case HELP:
                help();
                break;
            case TRUNCATE:
                truncate(argc, argv);
                break;
            default:
                printf("%s: option does not exist.\n", argv[1]);
        }
    }
    return EXIT_SUCCESS;
}