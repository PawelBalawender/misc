/* The C Programming Language, ex. 1-21
 * Replace all strings of blanks by the minimum number of tabs and blanks to
 * achieve the same spacing. Use the same tab stops as for detab
 */
#include<stdio.h>
#define TAB_WIDTH (4U)


void entab(char*);
void print_asc(char*);
void str_to_arr(char*, const char*);


/* Change in-place every sequence of tab_width blanks to a tab sign */
void entab(char* msg) {
    unsigned int i;

    /* With the first pointer we'll iter over the original string,
     * with the second we'll write new characters (modifying in-place) */
    char* original = msg;
    char* entabbed = original;
    
    /* a switch and a counter to detect sequences of blanks */
    unsigned int in_seq = 0;
    unsigned int seq_c = 0;

    while (*original) {
        if (*original == ' ' && in_seq == 0) in_seq = 1;
        if (*original == ' ' && in_seq) seq_c++;
        if (in_seq && *(original+1) != ' ') {

            for (i=0; i < seq_c / TAB_WIDTH; i++) *entabbed++ = '\t';
            for (i=0; i < seq_c % TAB_WIDTH; i++) *entabbed++ = ' ';
            
            in_seq = seq_c = 0;
            
            /* neutralize the folllowing incrementation */
            entabbed--;

        }

        /* carry on */
        if (in_seq == 0) entabbed++;
        original++;

        /* and rewrite the next char (it will never be a space, so it's ok) */
        *entabbed = *original;
    }
}

/* Print strings as an array of numerical values */
void print_asc(char* msg) {
    while (*msg) {
        printf("%d ", *msg);
        msg++;
    }
    printf("\n");
}

/* Take a constant string literal and save it to a mutable array */
void str_to_arr(char* arr, const char* str_literal) {
    const char* p = str_literal;
    char* a = arr;
    while (*p) {
        *a = *p;
        a++;
        p++;
    }
    *a = '\0';
}

/* The output should read:
 * 9\n                      1 tab
 * 9 32 32\n                1 tab, 2 spaces
 */
int main() {
    unsigned int i;
    char msgs[2][32];
    /* 4 spaces should transform to 1 tab */
    str_to_arr(msgs[0], "    ");
    /* 6 spaces should transform to 1 tab 2 spaces */
    str_to_arr(msgs[1], "      ");

    for (i=0; i < 2; i++) {
        entab(msgs[i]);
        print_asc(msgs[i]);
    }
    
    return 0;
}

