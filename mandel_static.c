#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <time.h>
#include "mpi.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    const int Dheight = 400;
    const int Dwidth = 400;
    const int Nrect = 40;
    const int Alloc = 1;
    const int Result = Dheight * Nrect;
    const int real_max = 2;
    const int real_min = -2;
    const int imag_max = 2;
    const int imag_min = -2;
    float scale_real = (real_max - real_min) / (float)Dwidth;
    float scale_imag = (imag_max - imag_min) / (float)Dheight;

    int np, me;
    int **MandleBuffer, *WorkerBuffer;
    int tag = 42;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    if (np < 2)
    {
        if (me == 0)
        {
            printf("You need at least 2 processes\n");
            MPI_Finalize();
            exit(0);
        }
    }

    WorkerBuffer = (int *)calloc(sizeof(int), Nrect + 2);
    MandleBuffer = (int **)calloc(sizeof(int *), Dheight);
    for (int h = 0; h < Dheight; h++)
    {
        MandleBuffer[h] = (int *)calloc(sizeof(int), Dwidth);
    }
    for (int h = 0; h < Dheight; h++)
    {
        for (int w = 0; w < Dwidth; w++)
        {
            MandleBuffer[h][w] = -1;
        }
    }

    if (me == 0)
    {
        int send_count = 0;
        int recv_count = 0;
        while (1)
        {
            for (int p = 1; p < np; p++)
            {
                int width = send_count * Nrect;
                MPI_Send(&width, Alloc, MPI_INT, p, tag, MPI_COMM_WORLD);

                send_count++;
                if (send_count >= Dwidth / Nrect)
                    break;
            }
            for (int tasks = 0; tasks < (np - 1) * Dheight; tasks++)
            {
                MPI_Recv(WorkerBuffer, Nrect + 2, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
                for (int w = 0; w < Nrect; w++)
                {
                    MandleBuffer[WorkerBuffer[1]][WorkerBuffer[0] + w] = WorkerBuffer[w + 2];
                }
                char name[] = "static";
                recv_count++;
                if (recv_count % 20 == 0)
                    write_ppm(name, recv_count, np, Dwidth, Dheight, MandleBuffer);
                if (recv_count >= Dwidth * Dheight / Nrect)
                    break;
            }
            if (recv_count >= Dwidth * Dheight / Nrect)
                break;
        }
        for (int p = 1; p < np; p++)
        {
            int kill = -1;
            MPI_Send(&kill, Alloc, MPI_INT, p, tag, MPI_COMM_WORLD);
        }

        printf("\nAll Tasks Finished\n");
        char name[] = "static_final";
        // write_ppm(name, recv_count, np, Dwidth, Dheight, MandleBuffer);
        printf("Writing Image Done\n");
        free(MandleBuffer);
        MPI_Finalize();
        exit(0);
    }

    else
    {
        int width_n;
        while (1)
        {
            MPI_Recv(&width_n, Alloc, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
            WorkerBuffer[0] = width_n;
            double timelog = MPI_Wtime();

            if (width_n == -1)
            {
                MPI_Finalize();
                free(WorkerBuffer);
                return 0;
            }

            for (int y = 0; y < Dheight; y++)
            {
                WorkerBuffer[1] = y;
                for (int x = width_n; x < (width_n + Nrect); x++)
                {
                    float complex c;
                    float c_real = real_min + (x * scale_real);
                    float c_imag = imag_min + (y * scale_imag);
                    c = CMPLXF(c_real, c_imag);
                    int color = calc_pixel(c);
                    WorkerBuffer[x - width_n + 2] = color;
                }
                MPI_Send(WorkerBuffer, Nrect + 2, MPI_INT, 0, tag, MPI_COMM_WORLD);
            }
            double total_time = MPI_Wtime() - timelog;
            printf("work %d, time: %f\n", width_n / Nrect, total_time);
        }
    }
}