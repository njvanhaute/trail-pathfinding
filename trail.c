#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct pgmPic_struct {
    int rows;
    int cols;
    int **pixels;
} pgmPic;

bool ValidSides(char *direction);
int FindMin(pgmPic *myPic);
int FindMax(pgmPic *myPic);
void GetDimensions(pgmPic *myPic, FILE *inputFile);
void GetPixels(pgmPic *myPic, FILE *inputFile);
void ApplyMultiplier(pgmPic *myPic, int min, double multiplier);
void GeneratePGM(pgmPic *myPic, char *fileName);
void FindPath(pgmPic *myPic, char startSide);

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Error: Incorrect number of arguments.\n");
        return -1;
    }
    if (strlen(argv[2]) != 3) {
        printf("Usage: ./trail [datafile] [direction]\n");
        return -1;
    }
    if (!ValidSides(argv[2])) {
        printf("Error: '%s' is not a valid direction.\n", argv[2]);
        return -1;
    }
    char *fileName = malloc(strlen(argv[1]) + 5);
    strcpy(fileName, argv[1]);
    char startSide = argv[2][0];  
    FILE *inputFile = fopen(fileName, "r");
    if (!inputFile) {
        printf("Error: Couldn't open file: '%s'\n", argv[1]);
        return -1;
    }
    pgmPic *myPic = malloc(sizeof(pgmPic));
    GetDimensions(myPic, inputFile);
    myPic->pixels = malloc(sizeof(int *) * myPic->rows);
    int i;
    for (i = 0; i < myPic->rows; i++) {
        myPic->pixels[i] = malloc(sizeof(int) * myPic->cols);
    }
    GetPixels(myPic, inputFile);
    fclose(inputFile);
    int max = FindMax(myPic);
    int min = FindMin(myPic);
    double multiplier = (200.0) / (max - min);
    ApplyMultiplier(myPic, min, multiplier);
    GeneratePGM(myPic, fileName);
    FindPath(myPic, startSide);
    strcat(fileName, "-trail");
    GeneratePGM(myPic, fileName);
    free(fileName);
    for (i = 0; i < myPic->rows; i++) {
        free(myPic->pixels[i]);
    }
    free(myPic->pixels);
    return 0;
}

bool ValidSides(char *direction) {
    if (strcmp("W-E", direction) == 0) {
        return true;
    }
    if (strcmp("E-W", direction) == 0) {
        return true;
    }
    if (strcmp("N-S", direction) == 0) {
        return true;
    }
    if (strcmp("S-N", direction) == 0) {
        return true;
    }
    return false;
}  

int FindMin(pgmPic *myPic) {
    int i, j;
    int min = myPic->pixels[0][0];
    for (i = 0; i < myPic->rows; i++) {
        for (j = 0; j < myPic->cols; j++) {
            min = (myPic->pixels[i][j] < min) ? myPic->pixels[i][j] : min;
        }
    }
    return min;
} 

int FindMax(pgmPic *myPic) {
    int i, j;
    int max = myPic->pixels[0][0];
    for (i = 0; i < myPic->rows; i++) {
        for (j = 0; j < myPic->cols; j++) {
            max = (myPic->pixels[i][j] > max) ? myPic->pixels[i][j] : max;
        }
    }
    return max;
}

void GetDimensions(pgmPic *myPic, FILE *inputFile) {
    int rows, cols;
    fscanf(inputFile, "%d %d", &rows, &cols);
    myPic->rows = rows;
    myPic->cols = cols; 
}

void GetPixels(pgmPic *myPic, FILE *inputFile) {
    int i, j;
    for (i = 0; i < myPic->rows; i++) {
        for (j = 0; j < myPic->cols; j++) {
            fscanf(inputFile, "%d", &(myPic->pixels[i][j]));
        } 
    }
}

void ApplyMultiplier(pgmPic *myPic, int min, double multiplier) {
    int i, j;
    for (i = 0; i < myPic->rows; i++) {
        for (j = 0; j < myPic->cols; j++) {
            myPic->pixels[i][j] = 255 - (int)((myPic->pixels[i][j] - min) * multiplier + 0.5);
        }
    }
}

void GeneratePGM(pgmPic *myPic, char *fileName) {
    char *newFile = malloc(strlen(fileName) + 5);
    strcpy(newFile, fileName);
    strcat(newFile, ".pgm");
    FILE* outputFile = fopen(newFile, "w");
    if (!outputFile) {
        printf("Error: Could not create file.\n");
        return;
    }
    fprintf(outputFile, "P2\n");
    fprintf(outputFile, "%d %d\n", myPic->cols, myPic->rows);
    fprintf(outputFile, "255\n");
    int i, j;
    for (i = 0; i < myPic->rows; i++) {
        for (j = 0; j < myPic->cols; j++) {
            fprintf(outputFile, "%d ", myPic->pixels[i][j]);
        }
        fprintf(outputFile, "\n");
    }
    fclose(outputFile);
    free(newFile);
} 

void FindPath(pgmPic *myPic, char startSide) {
    int i, pos;
    if (startSide == 'W') {
        pos = myPic->rows / 2;
        myPic->pixels[pos][0] = 0;
        for (i = 0; i < myPic->cols - 1; i++) {
            // Lower edge of matrix:
            if (pos == myPic->rows - 1) {
                int a = myPic->pixels[pos][i + 1];
                int b = myPic->pixels[pos - 1][i + 1];
                pos = (a < b) ? pos - 1 : pos;
            }
            // Upper edge of matrix:
            else if (pos == 0) {
                int a = myPic->pixels[pos][i + 1];
                int b = myPic->pixels[pos + 1][i + 1];
                pos = (a < b) ? pos + 1 : pos;
            }
            else {
                int origPos = pos;  
                int a = myPic->pixels[pos][i + 1];
                int b = myPic->pixels[pos + 1][i + 1];
                int c = myPic->pixels[pos - 1][i + 1];
                pos = a > b ? (a > c ? pos : pos - 1) : (b > c ? pos + 1 : pos - 1);
                if (origPos == pos - 1 && b == a) pos = origPos;
                if (origPos == pos + 1 && c == a) pos = origPos;
                if (origPos == pos + 1 && c == b) pos = origPos + 1;
            }
            myPic->pixels[pos][i + 1] = 0;
        }
    }
    else if (startSide == 'E') {
        pos = myPic->rows / 2;
        myPic->pixels[pos][myPic->cols - 1] = 0;
        for (i = myPic->cols - 1; i > 0; i--) {
            // Lower edge of matrix:
            if (pos == myPic->rows - 1) {
                int a = myPic->pixels[pos][i - 1];
                int b = myPic->pixels[pos - 1][i - 1];
                pos = (a < b) ? pos - 1 : pos;
            }
            // Upper edge of matrix:
            else if (pos == 0) {
                int a = myPic->pixels[pos][i - 1];
                int b = myPic->pixels[pos + 1][i - 1];
                pos = (a < b) ? pos + 1 : pos;
            }
            else {
                int origPos = pos;
                int a = myPic->pixels[pos][i - 1];
                int b = myPic->pixels[pos + 1][i - 1];
                int c = myPic->pixels[pos - 1][i - 1];
                pos = a > b ? (a > c ? pos : pos - 1) : (b > c ? pos + 1 : pos - 1);
                if (origPos == pos - 1 && b == a) pos = origPos;
                if (origPos == pos + 1 && c == a) pos = origPos;
                if (origPos == pos + 1 && c == b) pos = origPos + 1;
            }
            myPic->pixels[pos][i - 1] = 0;
        }
    }
    else if (startSide == 'S') {
        pos = myPic->cols / 2;
        myPic->pixels[myPic->rows - 1][pos] = 0;
        for (i = myPic->rows - 1; i > 0; i--) {
            // Right edge of matrix:
            if (pos == myPic->cols - 1) {
                int a = myPic->pixels[i - 1][pos];
                int b = myPic->pixels[i - 1][pos - 1];
                pos = (a < b) ? pos - 1 : pos;
            }
            // Left edge of matrix:
            else if (pos == 0) {
                int a = myPic->pixels[i - 1][pos];
                int b = myPic->pixels[i - 1][pos + 1];
                pos = (a < b) ? pos + 1 : pos;
            }
            else {
                int origPos = pos;
                int a = myPic->pixels[i - 1][pos];
                int b = myPic->pixels[i - 1][pos + 1];
                int c = myPic->pixels[i - 1][pos - 1];
                pos = a > b ? (a > c ? pos : pos - 1) : (b > c ? pos + 1 : pos - 1);
                if (origPos == pos - 1 && b == a) pos = origPos;
                if (origPos == pos + 1 && c == a) pos = origPos;
                if (origPos == pos + 1 && c == b) pos = origPos + 1;
            }
            myPic->pixels[i - 1][pos] = 0;
        }
    }
    else if (startSide == 'N') {
        pos = myPic->cols / 2;
        myPic->pixels[0][pos] = 0;
        for (i = 0; i < myPic->rows - 1; i++) {
            // Right edge of matrix:
            if (pos == myPic->cols - 1) {
                int a = myPic->pixels[i + 1][pos];
                int b = myPic->pixels[i + 1][pos - 1];
                pos = (a < b) ? pos - 1 : pos;
            }
            // Left edge of matrix:
            else if (pos == 0) {
                int a = myPic->pixels[i + 1][pos];
                int b = myPic->pixels[i + 1][pos + 1];
                pos = (a < b) ? pos + 1 : pos;
            }
            else {
                int origPos = pos;
                int a = myPic->pixels[i + 1][pos];
                int b = myPic->pixels[i + 1][pos + 1];
                int c = myPic->pixels[i + 1][pos - 1];
                pos = a > b ? (a > c ? pos : pos - 1) : (b > c ? pos + 1 : pos - 1);
                if (origPos == pos - 1 && b == a) pos = origPos;
                if (origPos == pos + 1 && c == a) pos = origPos;
                if (origPos == pos + 1 && c == b) pos = origPos + 1;
            }
            myPic->pixels[i + 1][pos] = 0;
        }
    }
}        
