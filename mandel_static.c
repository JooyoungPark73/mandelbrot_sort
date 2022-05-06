#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include "mpi.h"
#include "mandel_l.h"

int main(int argc, char *argv[])
{
    const int Dheight = 400;
    const int Dwidth = 400;
    const int Nrect = 40;
    const int Alloc = 1;
    const int Result = Dheight * Nrect;
    int np, me;
    int *MandleBuffer, *SlaveBuffer;
    int tag = 42;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    MPI_Barrier(MPI_COMM_WORLD);

    if (me == 0)
    {
        MandleBuffer = (int *)malloc(Dheight * Dwidth * sizeof(int));
        for (int worker = 0; worker < 10; worker++)
        {
            MPI_Send(worker, Alloc, MPI_INT, worker, tag, MPI_COMM_WORLD);
        }

        for (int worker = 0; worker < 10; worker++)
        {
            MPI_Recv();
        }
    }

    if (me != 0)
    {
    }
}