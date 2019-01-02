#include <stdio.h>
#define N (3U)
#define M (4U)
#define P (2U)

int main(int argc, char* argv[]) {
    /* C should be:
     * 1.001, 0
     * -0.19 0.999
     *  -0.061 0.001
     */

    float A[N][M] = {
        {0.1, 0.2, 0.0, 0.1},
        {0.2, 0.1, 0.3, 0.0},
        {0.0, 0.3, 0.1, 0.5},
    };

    float B[M][P] = {
        {4.92, 2.54},
        {3.02, -1.51},
        {-4.92, 2.14},
        {-0.95, 0.48}
    };

    float C[N][P] = {0};  // C: output matrix; N rows of P columns

    for (int i=0; i < N; ++i)
        for (int j=0; j < P; ++j)
            for (int k=0; k < M; ++k)
                C[i][j] += A[i][k] * B[k][j];

    for (int i=0; i < N; ++i){
        for (int j=0; j < P; ++j)
            printf("%f ", C[i][j]);
        printf("\n");
    }

    return 0;
}

