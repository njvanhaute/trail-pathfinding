#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct pgm_struct {
    int rows;
    int cols;
    int **pixels;
} pgmPic;

void GetDimensions(pgmPic *myPic, FILE *inputFile);
void GetPixels(pgmPic *myPic, FILE *inputFile);
void ApplyMultiplier(pgmPic *myPic, int min, double multiplier);
void GeneratePGM(pgmPic *myPic, char *fileName);
void GenerateTrail(pgmPic *myPic, char *fileName);
void FindPath(pgmPic *myPic, char startSide);
bool ValidSides(char *direction);
int FindMax(pgmPic *myPic);
int FindMin(pgmPic *myPic);

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Error: Incorrect number of arguments.\n");
        return -1;
    }
    if (strlen(argv[2]) != 3) {
        printf("Usage: ./trail [datafile] [direction]\n");
        return -1;
    }
    char startSide; 
    if (!ValidSides(argv[2])) {
        printf("Error: '%s' is not a valid direction.\n", argv[2]);
        return -1;
    }
    startSide = argv[2][0];  
    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        printf("Error: Couldn't open file: '%s'\n", argv[1]);
        return -1;
    }
    pgmPic *myPic = malloc(sizeof(pgmPic));
    GetDimensions(myPic, inputFile);
    myPic->pixels = malloc(sizeof(int *) * myPic->rows);
    int i = 0;
    for (i = 0; i < myPic->rows; i++) {
        myPic->pixels[i] = malloc(sizeof(int) * myPic->cols);
    }
    GetPixels(myPic, inputFile);
    fclose(inputFile);
    int max = FindMax(myPic);
    int min = FindMin(myPic);
    double multiplier = (200) / (max - min);
    ApplyMultiplier(myPic, min, multiplier);
    GeneratePGM(myPic, argv[1]);
    FindPath(myPic, startSide);
    GenerateTrail(myPic, argv[1]);
    return 0;
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
} 

void GenerateTrail(pgmPic *myPic, char *fileName) {
    char *newFile = malloc(strlen(fileName) + 5);
    strcpy(newFile, fileName);
    strcat(newFile, "-trail.pgm");
    FILE *outputFile = fopen(newFile, "w");
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
                pos = (a > b) ? pos : pos - 1;
            }
            // Upper edge of matrix:
            else if (pos == 0) {
                int a = myPic->pixels[pos][i + 1];
                int b = myPic->pixels[pos + 1][i + 1];
                pos = (a > b) ? pos : pos + 1;
            }
            else {
                int a = myPic->pixels[pos][i + 1];
                int b = myPic->pixels[pos + 1][i + 1];
                int c = myPic->pixels[pos - 1][i + 1];
                pos = a > b ? (a > c ? pos : pos - 1) : (b > c ? pos + 1 : pos - 1);
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
                pos = (a > b) ? pos : pos - 1;
            }
            // Upper edge of matrix:
            else if (pos == 0) {
                int a = myPic->pixels[pos][i - 1];
                int b = myPic->pixels[pos + 1][i - 1];
                pos = (a > b) ? pos : pos + 1;
            }
            else {
                int a = myPic->pixels[pos][i - 1];
                int b = myPic->pixels[pos + 1][i - 1];
                int c = myPic->pixels[pos - 1][i - 1];
                pos = a > b ? (a > c ? pos : pos - 1) : (b > c ? pos + 1 : pos - 1);
            }
            myPic->pixels[pos][i - 1] = 0;
        }
    }
    if (startSide == 'S') {
        pos = myPic->cols / 2;
        myPic->pixels[myPic->rows - 1][pos] = 0;
        for (i = myPic->rows - 1; i > 0; i--) {
            // Right edge of matrix:
            if (pos == myPic->cols - 1) {
                int a = myPic->pixels[i - 1][pos];
                int b = myPic->pixels[i - 1][pos - 1];
                pos = (a > b) ? pos : pos - 1;
            }
            // Left edge of matrix:
            else if (pos == 0) {
                int a = myPic->pixels[i - 1][pos];
                int b = myPic->pixels[i - 1][pos + 1];
                pos = (a > b) ? pos : pos + 1;
            }
            else {
                int a = myPic->pixels[i - 1][pos];
                int b = myPic->pixels[i - 1][pos + 1];
                int c = myPic->pixels[i - 1][pos - 1];
                pos = a > b ? (a > c ? pos : pos - 1) : (b > c ? pos + 1 : pos - 1);
            }
            myPic->pixels[i - 1][pos] = 0;
        }
    }
    if (startSide == 'N') {
        pos = myPic->cols / 2;
        myPic->pixels[0][pos] = 0;
        for (i = 0; i < myPic->rows - 1; i++) {
            // Right edge of matrix:
            if (pos == myPic->cols - 1) {
                int a = myPic->pixels[i + 1][pos];
                int b = myPic->pixels[i + 1][pos - 1];
                pos = (a > b) ? pos : pos - 1;
            }
            // Left edge of matrix:
            else if (pos == 0) {
                int a = myPic->pixels[i + 1][pos];
                int b = myPic->pixels[i + 1][pos + 1];
                pos = (a > b) ? pos : pos + 1;
            }
            else {
                int a = myPic->pixels[i + 1][pos];
                int b = myPic->pixels[i + 1][pos + 1];
                int c = myPic->pixels[i + 1][pos - 1];
                pos = a > b ? (a > c ? pos : pos - 1) : (b > c ? pos + 1 : pos - 1);
            }
            myPic->pixels[i + 1][pos] = 0;
        }
    }
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
