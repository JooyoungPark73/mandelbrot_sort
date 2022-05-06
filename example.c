#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    const int K = 1024;
    const int msgsize = 256 * K; /* Messages will be of size 1MB */
    int np, me, i;
    int *X, *Y;
    int tag = 42;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    /* Check that we run on exactly two processes */
    if (np != 2)
    {
        if (me == 0)
        {
            printf("You have to use exactly 2 processes to run this program \n");
        }
        MPI_Finalize();
        exit(0); /* Quit if there is only one process*/
    }
    /* Allocate memory for large message buffers */
    X = (int *)malloc(msgsize * sizeof(int));
    Y = (int *)malloc(msgsize * sizeof(int));
    /* Initialize X and Y */
    for (i = 0; i < msgsize; i++)
    {
        X[i] = 12345;
        Y[i] = me;
    }
    if (me == 0)
    {
        printf("Message size is %d bytes \n", msgsize * sizeof(int));
        printf("process %d sending to process 1 \n", me);
        MPI_Send(X, msgsize, MPI_INT, 1, tag, MPI_COMM_WORLD);
        printf("process %d receiving from process 1 \n", me);
        MPI_Recv(Y, msgsize, MPI_INT, 1, tag, MPI_COMM_WORLD, &status);
        printf("Y now has the value %d \n", Y[0]);
    }
    else
    {
        /* me == 1 */
        MPI_Recv(Y, msgsize, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Send(X, msgsize, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    exit(0);
}