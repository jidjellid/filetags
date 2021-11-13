#ifndef __HELPER__C__
#define __HELPER__C__

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

/**
 *  Concat method 
 */
char *concat(const char *s1, const char *s2){
    char *result = malloc(strlen(s1) + strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    strcat(result, "\0");
    return result;
}

/**
 *  Check if it's a file
 */
int isFile(const char *name)
{
    DIR *directory = opendir(name);
    if (directory != NULL)
    {
        closedir(directory);
        return 0;
    }
    else if (errno == ENOTDIR)
    {
        return 1;
    }
    return -1;
}

/**
 *  Avoid to go back and repete recursively actions 
 */
int notCurrent(char const *name)
{
    if (strcmp(name, "..") != 0 && strcmp(name, ".") != 0)
    {
        return 1;
    }
    return 0;
}

/**
 *  Get string from a file
 */
char *readString(const char *name)
{
    char buffer[BUFFER_SIZE];
    char *result = malloc(BUFFER_SIZE * 2);
    int file = open(name, O_RDONLY);
    int res = read(file, buffer, BUFFER_SIZE);
    while (res != 0)
    {
        strcat(result, buffer);
        res = read(file, buffer, BUFFER_SIZE);
    }
    close(file);
    strcat(result, "\0");
    return result;
}

/**
 *  Check if string start with a prefix
 */
int strStartWith(char const *pre, char const *str){
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

/**
 *  Get sub-string from string
 */
char* substr(const char *src, int m, int n)
{
	int len = n - m;
	char *dest = (char*)malloc(sizeof(char) * (len + 1));
	for (int i = m; i < n && (*(src + i) != '\0'); i++)
	{
		*dest = *(src + i);
        dest++;
	}
	*dest = '\0';
	return dest - len;
}

#endif