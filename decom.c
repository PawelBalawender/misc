/* The C Programming Language, Ex. 1-23, remove all comments from a C-string */
#include<stdio.h>

void decom(char*);

int main(void) {
    char data[] = "\
#include<stdio.h>/*Foobar*/\n\
\n\
int main(void) {\n\
    /**/\n\
    /*/**/\n\
    /*Just some arbitrary code */\n\
    return 0;\n\
}\n\
\n\
";
    decom(data);

    printf("%s", data);
    return 0;
}

void decom(char* code) {
    /* We will use this pointer to modify data in "code" */
    char* c = code;
    int done;

    while (*code) {
        /* omit the comment */
        if (*code == '/' && *(code + 1) == '*') {
            code++; /* in case of slash-asterisk-slash */
            do {
                code++;
                done = *code == '*' && *(code + 1) == '/';
            } while (done == 0);
            /* jump over the comment-end */
            code += 2;
        }
        *c++ = *code++;
    }
    *c = '\0';
}

