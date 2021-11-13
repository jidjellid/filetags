#include <sys/xattr.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include "config.c"
#include "helper.c"
#include "database.c"

#define SIZE 1024
#define ARG_FROM 3

/**
 * Check if a file contains a tag
 */
int exist(char const *file, char const *tag){
    int errno;
    char value[SIZE];
    char *xattr = concat(ATTR, getenv("USER"));
    getxattr(file, xattr, &value,  sizeof(value));
    free(xattr);
    //Skip not a directory error
    if(errno==20){
        return 1;
    }else if(errno){
        return 0;
    }
    if(strstr(value, tag) != NULL){
        return 1;
    }
    return 0;
}

/**
 * Get tags of a file
 * Boolean is for 2 cases: 
 *      true -> print
 *      false -> return
 */
char *getOne(char const *file, bool boolean){
    int errno;
    char value[SIZE];
    char *xattr = concat(ATTR, getenv("USER"));
    int size = getxattr(file, xattr, &value,  sizeof(value));
    free(xattr);
    if(errno){
        if(boolean){
            perror("tags#1");
        }
    }else{
        value[size]='\0';
        if(boolean){
            printf("Tags: \033[1;33m%s\033[0m had this/these tag(s) :\n\033[1;33m%s\033[0m.\n" ,file, value);
        }
    }

    if(boolean==false){
        char *result=malloc(strlen(value));
        strcpy(result, value);
        return result;
    }
    return "";
}

/**
 * General methode to get tags 
 * from an array of files
 */
void get(int argc, char const *argv[]){
    for (int i = ARG_FROM-1; i < argc; i++)
    {   
        getOne(argv[i], 1);
    }
}

/**
 * Add a tag to a file
 */
void addOne(char const *file, char const *tag){
    int errno;
    char values[SIZE] ="";
    char *xattr = concat(ATTR, getenv("USER"));
    getxattr(file, xattr, &values,  sizeof(values));
    strcat(values, tag);
    strcat(values, " ");
    removexattr(file, xattr);
    setxattr(file, xattr, values, strlen(values), XATTR_CREATE);
    if(errno && errno!=61){
        perror("add#1");
    }else{
        printf("Tag \033[1;33m%s\033[0m has been added to \033[1;33m%s\033[0m.\n", tag, file);
    }
    free(xattr);
}

/**
 * General method to add an array 
 * of tags to a file 
 */
void add(int argc, char const *argv[]){

    for (int i = ARG_FROM; i < argc; i++)
    {
        if(exist(argv[2], argv[i])==0){
            addOne(argv[2], argv[i]);
        }else{
            printf("Tag \033[1;32m %s \033[0m already exists on \033[1;33m%s\033[0m.\n", argv[i], argv[2]);
        }
    }
}

/**
 * Delete a tag from a file
 */
void deleteOne(char const *file, char const *tag){
    int errno;
    char values[SIZE] = "";
    char result[SIZE] = "";
    char *xattr = concat(ATTR, getenv("USER"));
    getxattr(file, xattr, &values,  sizeof(values));
    if(errno){
        perror("remove#1");
    }else{
        char * token = strtok(values, " ");
        while( token != NULL ) {
            if(strcmp(token, tag)!=0){
                strcat(result, token); 
                strcat(result, " "); 
            }
            token = strtok(NULL, " ");
        }
        strcat(result, "\0");
        removexattr(file, xattr);
        setxattr(file, xattr, result, strlen(result), XATTR_CREATE);
        printf("Tag \033[1;33m%s\033[0m has been deleted from \033[1;33m%s\033[0m.\n", tag, file);
    }
    free(xattr);
}

/**
 * General method to delete an array 
 * of tags from a file
 */
void delete(int argc, char const *argv[]){
    for (int i = ARG_FROM; i < argc; i++)
    {
        if(exist(argv[2], argv[i])){
            deleteOne(argv[2], argv[i]);
        }else{
            printf("Tag \033[1;31m%s\033[0m don't exists on \033[1;33m%s\033[0m.\n", argv[i], argv[2]);
        }
    }
}

/**
 * Delete all tags from a file
 */
void truncate(int argc, char const *argv[]){
    for (int i = ARG_FROM-1; i < argc; i++)
    {
        int errno;
        char *xattr = concat(ATTR, getenv("USER"));
        removexattr(argv[i], xattr);
        free(xattr);
        if(errno){
            perror("truncate");
        }else{
            printf("All tags have been deleted from \033[1;33m%s\033[0m.\n", argv[i]);
        }
    } 
}


/*
 * Extract the tags to contain when bool ==  1 and for bool == 0 the tags to exclude
 */
char *extract(int argc, char const *argv[], bool boolean){
    char *negation = "pas";
    char *result = malloc(SIZE);
    strcpy(result, "");
    for (int i = ARG_FROM-1; i < argc; i++)
    {
        if(strcmp(argv[i], "et")!=0){
            if(boolean == false && strStartWith(negation, argv[i])){
                char *sub = substr(argv[i], 4, strlen(argv[i])-1);
                strcat(result, sub);
                strcat(result, " ");
            }else if(boolean == true && strStartWith(negation, argv[i])!=1){
                strcat(result, argv[i]);
                strcat(result, " ");
            }
        }
    }
    strcat(result, "\0");
    return result;
}

/*
 * Returns 1 when values contains only tags_to_contain and 0 when values contains tags_to_exclude
 */
int contains(char *values, char *tags_to_contains, char *tags_to_exclude){
    const char *sep = " ";
    int positive, negative, total_positive, total_negative;
    positive = negative = total_positive = total_negative = 0;
    char *cont = strtok(tags_to_contains,sep);
    while (cont != NULL){
        if(strstr(values,cont) != NULL)
            positive++;
        cont = strtok(NULL,sep);
        total_positive++;
    }
    char *excl = strtok(tags_to_exclude,sep);
    while(excl != NULL){
        if(strstr(values,excl) == NULL)
            negative++;
        excl = strtok(NULL,sep);
        total_negative++;
    }
    return positive==total_positive && negative==total_negative;
}


/**
 * Get all tags of an file with 
 * the hierachical application 
 * from database
 */
char *allTags(char const *file){
    char *values = malloc(SIZE);
    strcpy(values, "");
    char *local = getOne(file, 0);
    char *token = strtok(local, " ");     
    while(token != NULL) {
        char *hierachy = from_child_to_parent(token);
        strcat(values, strlen(hierachy)!=0 ? hierachy : token);
        strcat(values, " ");
        token = strtok(NULL, " ");
        free(hierachy);
    }
    free(local);
    strcat(values, "\0");
    return values;
}

/**
 *  Search files which contain tags of the query
 */
void searchOne(int argc, char const *argv[], char const *path){
    int i = 0;
    struct dirent *dp;
    DIR *dir = opendir(path);
    if (!dir) 
        return; 
    while ((dp = readdir(dir)) != NULL){
        if(notCurrent(dp->d_name)==1 && isFile(dp->d_name))
        {   
            char *values = allTags(dp->d_name);
            char *positive = extract(argc, argv, 1);
            char *negative = extract(argc, argv, 0);
            if(contains(values, positive, negative)){
                printf("\033[1;36m%s\033[0m   ", dp->d_name);
                i++;
            }
            free(values);
            free(positive);
            free(negative);
        }
    }
    if(i==0)
        printf("Any result...");
    closedir(dir);
}

/**
 *  General function for seach command
 */
void search(int argc, char const *argv[]){
    if(argc>2){
        printf("Files for your query: ");
        for (size_t i = ARG_FROM-1; i < argc; i++)
        {
            printf("\033[1;32m%s\033[0m ", argv[i]);
        }
        printf("\n");
        searchOne(argc, argv, ".");
    }
    printf("\n");
}

/**
 * Add hierachical rule
 */
void addgrad(int argc, char const *argv[]){
    if(argc==4)
        insert_db(argv[2],argv[3]);
    else
        printf("Only 2 params!");
}

/**
 * Delete hierachical rule
 */
void deletegrad(int argc, char const *argv[]){
    if(argc==3)
        delete_db(argv[2]);
    else
        printf("Only 1 param!");
}

/**
 * Display tree of the hierarchy
 */
void tree(int argc, char const *argv[]){
    if(argc==3)
        printTree(argv[2]);
    else
        avaible();
}

/**
 * Display the help page
 */
void help(){
    char *path = concat(ROOT, DOCS);
    char *help = readString(path);
    printf("%s\n",help);
    free(help);
    free(path);
}
