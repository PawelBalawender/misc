/* The C Programming Language Exercise 1-22 */
#include<stdio.h>
#define MAX_SIZE 1024
#define MAX_LINE 8

char* folded(char*);

int main(void) {
    /* Those tests should result in 7 "01234567" lines on stdout */
    printf("%s\n", folded("01234567"));*/
    printf("%s\n", folded("012345670123456701234567"));*/
    printf("%s\n", folded("01234567\n01234567\n01234567"));
    return 0;
}

char* folded(char* long_line) {
    static char folded_line[MAX_SIZE];
    char* c = &folded_line[0];
    int counter = 0;

    while (*long_line) {
        if (counter && counter % MAX_LINE == 0 && *long_line != '\n') *c++ = '\n';    
        *long_line == '\n' ? counter = 0 : counter++;
        *c++ = *long_line++;
    }
    return folded_line;
}

