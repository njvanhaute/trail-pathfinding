#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int LIMIT = 5;

int main(void) {
    srand(time(NULL));
    int i, j;
    FILE *outputFile = fopen("random", "w");
    int rows = rand() % LIMIT + LIMIT;
    int cols = rand() % LIMIT + LIMIT;
    fprintf(outputFile, "%d\n%d\n", rows, cols);
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            fprintf(outputFile, "%d ", rand() % LIMIT + LIMIT);
        }
        fprintf(outputFile, "\n");
    }
    fclose(outputFile);
    return 0;
}
