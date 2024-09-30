// gcc -D__TEST_NAME__=allocate basic-test-suite.c matrix.c unity.c -o allocate_test
// gcc basic-test-suite.c matrix.c unity.c -o all_tests

#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h> /* for ENOSYS */

void print_matrix(matrix_t *m) {
    printf("\n");
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            printf("%d ", m->content[i][j]);
        }
        printf("\n");
    }
}

int matrix_allocate(matrix_t *m, int rows, int columns) {
    if (rows == 0 && columns == 0) {
        return 0;
    }
    m->rows = rows;
    m->columns = columns;

    // content is a pointer to each containing a reference to each row, so we allocate rows * size of an int pointer because we want rows number of pointers to int arrays.
    m->content = (int **) malloc(rows * sizeof(int *));
    if (m->content == NULL) {
        return -1;
    }

    /* now we allocate memory for each row
     * each content[i] is the same as *(content + i) which is a pointer to an array of integers
     * therefore we allocate columns * size of an int for each row, and each content[i] points to this 
    */ 
    for (int i = 0; i < rows; i++) {
        m->content[i] = (int *) malloc(columns * sizeof(int));
        if (m->content[i] == NULL) {
            // Handle memory allocation failure
            // Free previously allocated memory
            for (int j = 0; j < i; j++) {
                free(m->content[j]);
            }
            free(m->content);
            return -ENOMEM;
        }
    }

    // some sanity checks
    // printf("Rows: %d, Columns: %d\n", rows, columns);
    // printf("Size of m (the struct) is: %lu\n", sizeof(*m));
    // printf("Size of the pointer to m is: %lu\n", sizeof(m));
    // printf("Size of each content pointer is: %lu -- We have 10 of these, one for each row.\n", sizeof(m->content[1]));
    // printf("Size of the what the content pointer points to is: %lu\n", sizeof(*((m->content[0]) + 1)));
    return 0;
}

// free the memory allocated for the matrix
void matrix_free(matrix_t *m) {
    // first we free each row
    for (int i = 0; i < m->rows; i++) {
        free(m->content[i]);
    }
    // then free the pointer to the rows
    free(m->content);
    m->content = NULL;
    m->rows = 0;
    m->columns = 0;
}

void matrix_init_n(matrix_t *m, int n) {
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            m->content[i][j] = n;
        }
    }
    // print_matrix(m);
}

void matrix_init_zeros(matrix_t *m) {
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            m->content[i][j] = 0;
        }
    }
    // print_matrix(m);
}

int matrix_init_identity(matrix_t *m){
    // identity matrix must be square
    if (m->rows != m->columns) {
        return -1;
    }

    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            if (i == j) {
               m->content[i][j] = 1;
            } else {
                m->content[i][j] = 0;
            }
        }
    }
    // print_matrix(m);
    return 0;
}

int matrix_init_rand(matrix_t *m, int val_min, int val_max) {
    if (val_min > val_max) {
        return -1;
    }
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            m->content[i][j] = rand() % (val_max - val_min + 1) + val_min;
        }
    }
    // print_matrix(m);
    return 0;
}

int matrix_equal(matrix_t *m1, matrix_t *m2) {
    // initial check for same dimensions
    if (m1->rows != m2->rows || m1->columns != m2->columns) {
        return 0;
    }

    // check all cells
    for (int i = 0; i < m1->rows; i++) {
        for (int j = 0; j < m1->columns; j++) {
            if (m1->content[i][j] != m2->content[i][j]) {
                return 0;
            }
        }
    }
    return 1;
}

int matrix_sum(matrix_t *m1, matrix_t *m2, matrix_t *result) {
    if (m1->rows != m2->rows || m1->columns != m2->columns) {
        return -1;
    }
    matrix_allocate(result, m1->rows, m1->columns);
    for (int i = 0; i < m1->rows; i++) {
        for (int j = 0; j < m1->columns; j++) {
            result->content[i][j] = m1->content[i][j] + m2->content[i][j];
        }
    }
    // print_matrix(result);
    return 0;
}

int matrix_scalar_product(matrix_t *m, int scalar, matrix_t *result) {
    matrix_allocate(result, m->rows, m->columns);
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            result->content[i][j] = m->content[i][j] * scalar;
        }
    }
    print_matrix(result);
    return 0;
}

// calculate the transpose of a matrix
int matrix_transposition(matrix_t *m, matrix_t *result) {
    matrix_allocate(result, m->columns, m->rows);
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            result->content[j][i] = m->content[i][j];
        }
    }
    // print_matrix(m);
    // print_matrix(result);
    return 0;
}

// multiply two matrices
int matrix_product(matrix_t *m1, matrix_t *m2, matrix_t *result) {
    // requirement for multiplication of two matrices
    if (m1->columns != m2->rows) {
        return -1;
    }

    matrix_allocate(result, m1->rows, m2->columns);
    for (int i = 0; i < m1->rows; i++) {
        for (int j = 0; j < m2->columns; j++) {
            result->content[i][j] = 0;
            for (int k = 0; k < m1->columns; k++) {
                result->content[i][j] += m1->content[i][k] * m2->content[k][j];
            }
        }
    }
    print_matrix(result);
    return 0;
}

// this should write the matrix to a file in the format described in the subject
int matrix_dump_file(matrix_t *m, const char *output_file) {
    FILE *file = fopen(output_file, "w");
    if (file == NULL) {
        return -1;
    }
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->columns; j++) {
            fprintf(file, "%d ", m->content[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
    return 0;
}

int get_cols(const char *line) {
    int count = 0;
    int in_word = 0;
    while (*line) {
        if (isspace(*line)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        line++;
    }
    return count;
}

int matrix_allocate_and_init_file(matrix_t *m, const char *input_file) {
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        return -1;
    }

    char buffer[1024];
    int rows = 0;
    int cols = 0;
    
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline character
        cols = get_cols(buffer);
        rows++;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        rows++;
    }

    fclose(file);
    
    if (matrix_allocate(m, rows, cols) != 0) {
        return -1;
    }

    // Reopen the file to read the matrix data
    file = fopen(input_file, "r");
    if (file == NULL) {
        return -1;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, "%d", &m->content[i][j]);
        }
    }

    fclose(file);
    print_matrix(m);
    return 0;
}