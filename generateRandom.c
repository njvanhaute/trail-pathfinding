#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int LIMIT = 7;

int main(void) {
    srand(time(NULL));
    int i = 0;
    int j = 0;
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
