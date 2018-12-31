#include<stdio.h>
#include<errno.h>


int main(int argc, char* argv[]) {
    FILE *fp;
    fp = fopen("bf.s", "w");
    if (fp == NULL) {
        printf("Output file not created, errno = %d\n", errno);
        return 1;
    }
    // fprintf(fp, "Hi!"); to write somtehting
    fclose(fp);
    
    char prog[1024] = "++++++++[>+++++++++<-]>.>---."
    return 0;
}

