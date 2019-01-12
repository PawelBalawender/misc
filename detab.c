/* The C programming language, ex. 1-20
 * Write a program detab that replaces tabs in the input with the proper number
 * of blanks to space to the next tab stop. Assume a fixed set of tab stops, say
 * every n columns.
 */
#include<stdio.h>
#define MAX_SIZE 1024
#define TAB_STOP 4

char* detabbed(char*);

/* Output should read:
 * "    !"
 * "    !"
 * "!   !"
 * "!!! !"
 * "!!!!!   !"
 */
int main(void) {
    unsigned int i;
    char* strings[5];
    strings[0] = "    !";
    strings[1] = "\t!";
    strings[2] = "!\t!";
    strings[3] = "!!!\t!";
    strings[4] = "!!!!!\t!";
    
    for (i=0; i < 5; i++) {
        printf("%s\n", detabbed(strings[i]));
    }
    return 0;
}

/* Change every tab in the input string to number of spaces sufficient to
 * reach the next tab stop
 */
char* detabbed(char* original) { 
    unsigned int i;
    static char container[MAX_SIZE];
    char* c = container;
    
    /* number of chars on the line */
    int count = 0;
    
    while(*original) {
        /* insert a proper amount of spaces */
        if (*original == '\t') {
            for (i=0; i < TAB_STOP - count % TAB_STOP; i++) {
                *c = ' ';
                c++;
            }
            /* decrement to neutralize the incrementation below */
            c--;
        } else {
            if (*c == '\n') count = 0;
            *c = *original;
        }

        count++;
        original++;
        c++;
    }

    return container;
}

