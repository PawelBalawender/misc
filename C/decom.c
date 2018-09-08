/* The C Programming Language, ex. 1-23
 * Write a program to remove all comments from a C program. Don't forget
 * to handle quoted strings and character constants properly.
 * C comments don't nest
 */
#include<stdio.h>


void decom(char*);


/* Output should read (padded a few tabs to the right):
 * 
 * #include<stdio.h
 * int main(void) {
 *
 *     char s[] = "ANot a comment, in a stringB";
 *     return 0;
 * }
 * 
 * where A = slash-asterisk, comment begin
 *       B = asterisk-slash, comment end
 */
int main(void) {
    char data[] = "\
                    #include<stdio.h>/*Foobar*/\n\
                    int main(void) {\n\
                        /**//*/**/\n\
                        char s[] = \"/*Not a comment, in a string*/\";/*Just some arbitrary code */\n\
                        return 0;\n\
                    }\n";
    decom(data);

    printf("%s", data);
    return 0;
}

/* Remove all C-style comments from the given string */
void decom(char* code) {
    /* We will use this pointer to modify data in "code" */
    char* c = code;
    int done;
    int in_string = 0;

    while (*code) {
        /* isn't there any string? */
        if (*code == '"' && *(code - 1) != '\\') in_string ^= 1;

        /* isn't there any comment? */
        if (in_string == 0 && *code == '/' && *(code + 1) == '*') {
            /* jump over the slash, enter loop, jump over the asterisk */
            code++;
            
            do {
                code++;
                done = *code == '*' && *(code + 1) == '/';
            } while (done == 0);

            /* jump out of the comment */
            code += 2;
            /* otherwise we'll blindly asign the next char to our new array
             * - but there can be a new comment */
            continue;
        }
        *c++ = *code++;
    }
    *c = '\0';
}

