#include<stdio.h>
#define MAX_DATA (100000U)

int longest(int[], int[], int, int);
int avaliable_parities(int[], int, int);


int main(int argc, char* argv[]) {
    int q, i, j=0, m, n, a_data[MAX_DATA], b_data[MAX_DATA];
    
    scanf("%d", &q);
    for (i=0; i < q; i++) {
        scanf("%d %d", &m, &n);

        do {
            scanf("%d", &a_data[j++]);
        } while (j < m);

        j = 0;
        do {
            scanf("%d", &b_data[j++]);
        } while (j < n);

        if (m < n) {
            printf("%d\n", longest(a_data, b_data, m, n));
        } else {
            printf("%d\n", longest(b_data, a_data, n, m));
        }

    }
    return 0;
}


int longest(int shorter[], int longer[], int s_len, int l_len) {
    int pa, pb, len_goal;
    len_goal = s_len;

    while (1) {
        pa = avaliable_parities(shorter, s_len, len_goal);
        pb = avaliable_parities(longer, l_len, len_goal);
        if (pa==pb || pa==2 || pb==2) break;
        else --len_goal;
    }

    return len_goal;
}


int avaliable_parities(int s[], int s_len, int len_goal) {
    int i=0, s_sum=0, par, flag=0;

    for(int j=0; j < len_goal; j++) s_sum += s[j];
    par = s_sum % 2;

    while (i+len_goal < s_len) {
        if ((s[i + len_goal] - s[i]) % 2) {
            return 2;
        } else {
            ++i;
        }
    }

    return par;
}

