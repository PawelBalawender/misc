/* The C Programing Language, ex. 1-24
 * Write a program to check a C program for rudimentary syntax errors like
 * unmatched parentheses, brackets and braces. Don't forget about quotes,
 * both single and double, escape sequences and comments. (This program is hard
 * if you do it in full generality)
 */
#include<stdio.h>
#define CASES 7

int checksx(char*);

/* Output should read:
 * 0011001
 * what means that code is ok in cases 0,1,4,5, but it's not in cases 2,3,6
 */
int main(void) {
    unsigned int i;
    char* code[CASES];
    code[0] = "";
    code[1] = "(({[]}))";
    code[2] = "(();";
    code[3] = "())";
    code[4] = "char a[64] = \"nawiasy: ({[\"";
    code[5] = "char a = \'(\'";
    code[6] = "(({}[))";
    for (i=0; i < CASES; i++) {
        printf("%d", checksx(code[i]));
    }
    printf("\n");

    return 0;
}

int checksx(char* code) {
    unsigned int in_string = 0, in_char = 0;
    unsigned char scopes[64] = {0};
    unsigned char* s = scopes;
    while (*code) {
        switch (*code) {
            case '"':
                if (*(code - 1) != '\\') in_string ^= 1;
                break;
            case '\'':
                if (*(code - 1) != '\\') in_char ^= 1;
                break;
            case '(':
                if ((in_string || in_char) == 0) *++s = '(';
                break;
            case '[':
                if ((in_string || in_char) == 0) *++s = '[';
                break;
            case '{':
                if ((in_string || in_char) == 0) *++s = '{';
                break;
            case ')':
                if (*s != '(') return 1;
                *s-- = 0;
                break;
            case ']':
                if (*s != '[') return 1;
                *s-- = 0;
                break;
            case '}':
                if (*s != '{') return 1;
                *s-- = 0;
                break;
            default:
                break;
        }
        code++;
    }
    /* unclosed parenthesis/bracket/brace left */
    if (scopes[1]) return 1;
    return 0;
}

