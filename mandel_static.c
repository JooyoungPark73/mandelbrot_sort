#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include "mpi.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    const float Dheight = 400;
    const float Dwidth = 400;
    const int Nrect = 40;
    const int Alloc = 1;
    const int Result = Dheight * Nrect;
    const int real_max = 2;
    const int real_min = -2;
    const int imag_max = 2;
    const int imag_min = -2;
    float scale_real = (real_max - real_min) / Dwidth;
    float scale_imag = (imag_max - imag_min) / Dheight;

    int np, me;
    int **MandleBuffer, *SlaveBuffer;
    int tag = 42;

    SlaveBuffer = (int *)malloc((Dwidth + 2) * sizeof(int));
    MPI_Status status;

    MPI_Init(&argc, &argv);
    printf("MPI INIT COMPLETE");
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    if (me == 0)
    {
        MandleBuffer = (int **)malloc(Dheight * sizeof(int *));
        for (int h = 0; h < Dheight; h++)
        {
            MandleBuffer[h] = (int *)malloc(Dwidth * sizeof(int));
        }
        int send_count = 0;
        int recv_count = 0;
        while (1)
        {
            for (int p = 1; p < np; p++)
            {
                int width = send_count * 10;
                MPI_Send(&width, Alloc, MPI_INT, p, tag, MPI_COMM_WORLD);
                printf("Allocated Job %d\r", p);
                send_count++;
                if (send_count > Dwidth / 10)
                    break;
            }

            for (int tasks = 0; tasks < np * Dheight; tasks++)
            {
                MPI_Recv(SlaveBuffer, 11, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
                for (int w = 2; w < 12; w++)
                {
                    MandleBuffer[SlaveBuffer[1]][SlaveBuffer[0] + w] = SlaveBuffer[w];
                }
                if (recv_count > Dwidth * Dheight / 10)
                    break;
            }
        }
        printf("\nAll Tasks Finished\n");
        write_ppm(Dwidth, Dheight, MandleBuffer);
        printf("Writing Image Done\n");
        MPI_Finalize();
        exit(0);
    }

    if (me != 0)
    {
        int width_n;
        while (1)
        {
            MPI_Recv(&width_n, Alloc, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            SlaveBuffer[0] = width_n;
            for (int x = 0; x < Dheight; x++)
            {
                SlaveBuffer[1] = x;
                for (int y = width_n; y < (width_n + 10); y++)
                {
                    float complex c;
                    float c_real = real_min + (x * scale_real);
                    float c_imag = imag_min + (y * scale_imag);
                    c = CMPLXF(c_real, c_imag);
                    int color = calc_pixel(c);
                    SlaveBuffer[y - width_n + 2] = color;
                }
                MPI_Send(SlaveBuffer, 11, MPI_INT, 0, tag, MPI_COMM_WORLD);
            }
            printf("Task %d Finished", width_n);
        }
    }
}