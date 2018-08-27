/*
"The C programming language", Ex. 1-20
Write a program detab that replaces tabs in the input with the proper number
of blanks to space to the next tab stop. Assume a fixed set of tab stops, say
every n columns. Shuld n be a variable or a symbolic parameter?"
*/
#include<stdio.h>
#define MAX_SIZE 1024
#define TAB_STOP 4

static int i;

char* get_data(void);
void detab(char* arr, char* container);

/* Output the obtained by get_data() string with tabs expanded */
int main(void) {
    /* It's a test; exclamation mark should be in the same column in each of
     * cases [0:4] and in each of [4:5]
     */
     
    char expanded[MAX_SIZE * TAB_STOP];
    char* data[6];
    data[0] = "\t!";
    data[1] = "1\t!";
    data[2] = " 2\t!";
    data[3] = "  3\t!";
    data[4] = "   4\t!";
    data[5] = "      7\t!";
    for (i=0; i < 6; i++) {
        detab(data[i], expanded);
        printf("%s\n", expanded);
    }
    return 0;
}

/* Obtain the data: from a file, STDIN or a const like in this case */
char* get_data(void) {
    char* arr = "Hello,\tworld!\n";
    return arr;
}

/* Change every tab in the input string to number of spaces sufficient to
 * reach the next tab stop
 */
void detab(char* raw, char* container) {
    /* count holds the number of chars on the line to find the closest tabstop */
    int count=0;
    while(*raw) {
        /* insert a proper amount of spaces */
        if (*raw == '\t') {
            for (i=0; i < TAB_STOP - count % TAB_STOP; i++) {
                *container = ' ';
                container++;
            }
            /* decrement to neutralize the incrementation below */
            container--;
        } else {
            if (*raw == '\n') count = 0;
            *container = *raw;
        }

        count++;
        raw++;
        container++;
    }
}

