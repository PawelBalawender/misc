#include<stdio.h>
#define TAB_WIDTH 4

int i;

/* Change in-place every sequence of tab_width blanks to a tab sign */
void encode(char* msg, int tab_width) {
    /* With the first pointer we'll iter over the original string,
     * with the second we'll write new characters (modifying in-place) */
    char* original = msg;
    char* encoded = original;
    
    // a switch and a counter to detect sequences of blanks
    int in_seq = 0;
    int seq_c = 0;

    while (*original) {
        if (*original == ' ' && in_seq == 0) in_seq = 1;
        if (*original == ' ' && in_seq) seq_c++;
        if (in_seq && *(original+1) != ' ') {
            for (i=0; i < seq_c / tab_width; i++) *encoded++ = '\t';
            for (i=0; i < seq_c % tab_width; i++) *encoded++ = ' ';
            
            in_seq = seq_c = 0;
            
            // neutralize the folllowing incrementation
            encoded--;
        }

        // carry on
        if (in_seq == 0) encoded++;
        original++;

        // and rewrite the next char (it will never be a space, so it's ok)
        *encoded = *original;
    }
}

void print_asc(char* msg) {
    while (*msg) {
        printf("%3d ", *msg);
        msg++;
    }
    printf("\n");
}
 

int main() {
    // test 1: example to compare the following
    char msg1[] = "Hello,123456World";
    // test 2: 4 spaces should transform to 1 tab
    char msg2[] = "Hello,    World";
    // test 3: 6 spaces should transform to 1 tab 2 spaces
    char msg3[] = "Hello,      World";
    
    char* msgs[] = {msg1, msg2, msg3};
    for (int i=0; i < 3; i++) {
        encode(msgs[i], TAB_WIDTH);
        print_asc(msgs[i]);
    }

    return 0;
}

