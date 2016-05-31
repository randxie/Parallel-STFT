// from http://scicomp.stackexchange.com/questions/1573/is-there-an-mpi-all-gather-operation-for-matrices
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void printMatrix (float **m, int rows, int cols)
{
    for (int i = 0; i < rows; ++i) {
        printf ("%3d: ", i);
        for (int j = 0; j < cols; ++j)
            printf ("%2.0f ", m[i][j]);
        printf ("\n");
    }
}

float **allocMat (int rows, int cols)
{
    float  *data   = (float  *) malloc (rows * cols * sizeof(float));
    float **matrix = (float **) malloc (rows * sizeof(float *));
    for (int i = 0; i < rows; i++)
        matrix[i] = & (data[i * cols]);
    return matrix;
}

int main (int argc, char *argv[])
{
    int   size, rank;
    int   i, j;
    const int root = 0;
    const int globalncols = 10, globalnrows = 10;
    int ncols, start;
    int *allncols, *allstarts;
    float **matrix;
    MPI_Datatype columnunsized, column;

    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    /* everyone's number of columns and offsets */
    allncols = new int[rank];
    allstarts= new int[rank];

    /* everyone gets a global matrix */
    matrix = allocMat(globalnrows, globalncols);

    for (i = 0; i < globalnrows; i++)
        for (j = 0; j < globalncols; j++)
            matrix[i][j] = ( i == j? 1. : 0.);

    /* rank 0 print the results */
    if (rank == 0) {
        printf("Before:\n");
        printMatrix(matrix, globalnrows, globalncols);
    }


    /* how many columns are we responsble for? */
    ncols = (globalncols + rank)/size;

    MPI_Allgather(&ncols, 1, MPI_INT, allncols, 1, MPI_INT, MPI_COMM_WORLD);

    start = 0;
    for (int i=0; i<rank; i++)
        start += allncols[i];

    MPI_Allgather(&start, 1, MPI_INT, allstarts, 1, MPI_INT, MPI_COMM_WORLD);

    /* create the data type for a column of data */
    int sizes[2]    = {globalnrows, globalncols};
    int subsizes[2] = {globalnrows, 1};
    int starts[2]   = {0,0};
    MPI_Type_create_subarray (2, sizes, subsizes, starts, MPI_ORDER_C,
                              MPI_FLOAT, &columnunsized);
    MPI_Type_create_resized (columnunsized, 0, sizeof(float), &column);
    MPI_Type_commit(&column);


    /* everyone update their columns by adding their rank to all values */

    for (int row=0; row<globalnrows; row++)
        for (int col=start; col<start+ncols; col++)
            matrix[row][col] += rank;

    /* gather the updated columns */

    MPI_Allgatherv(&(matrix[0][start]), ncols, column,
                   &(matrix[0][0]), allncols, allstarts,
                   column, MPI_COMM_WORLD);

    /* rank 0 print the results */
    if (rank == 0) {
        printf("After:\n");
        printMatrix(matrix, globalnrows, globalncols);
    }

    MPI_Type_free (&column);
    free (matrix[0]);
    free (matrix);

    MPI_Finalize();
    return 0;
}