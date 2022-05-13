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
    const int total_send = Dwidth / Nrect;
    const int total_receive = Dheight * Nrect;
    const int real_max = 2;
    const int real_min = -2;
    const int imag_max = 2;
    const int imag_min = -2;
    float scale_real = (real_max - real_min) / (float)Dwidth;
    float scale_imag = (imag_max - imag_min) / (float)Dheight;

    char name[] = "static";
    int np, me;
    int **MandleBuffer, *WorkerBuffer;
    int DATA_TAG = 4;
    int ALLOCATE_TAG = 8;
    int KILL_TAG = 9;
    int FIN_TAG = 44;
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
        int fin_count = 0;
        double **log_array = (double **)calloc(sizeof(double *), 2);
        for (int h = 0; h < 2; h++)
        {
            log_array[h] = (double *)calloc(sizeof(double), total_send + 1);
        }

        log_array[0][total_send] = MPI_Wtime();
        while (send_count < np - 1)
        {
            MPI_Send(&send_count, Alloc, MPI_INT, send_count + 1, ALLOCATE_TAG, MPI_COMM_WORLD);
            log_array[0][send_count] = MPI_Wtime();
            send_count++;
        }
        while (fin_count < total_send)
        {
            MPI_Recv(WorkerBuffer, Nrect + 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == FIN_TAG)
            {
                log_array[1][WorkerBuffer[0]] = MPI_Wtime();
                fin_count++;
                int new_job = WorkerBuffer[0] + np - 1;
                if (new_job < total_send)
                {
                    MPI_Send(&new_job, Alloc, MPI_INT, status.MPI_SOURCE, ALLOCATE_TAG, MPI_COMM_WORLD);
                    log_array[0][new_job] = MPI_Wtime();
                    send_count++;
                }
            }

            if (status.MPI_TAG == DATA_TAG)
            {
                recv_count++;
                for (int w = 0; w < Nrect; w++)
                {
                    MandleBuffer[WorkerBuffer[1]][WorkerBuffer[0] * Nrect + w] = WorkerBuffer[w + 2];
                }
                if (recv_count % 20 == 0)
                    write_ppm(name, recv_count, np, Dwidth, Dheight, MandleBuffer);
            }
        }

        for (int p = 1; p < np; p++)
        {
            int kill = -1;
            MPI_Send(&kill, Alloc, MPI_INT, p, KILL_TAG, MPI_COMM_WORLD);
        }
        log_array[1][total_send] = MPI_Wtime();

        char name[] = "static_final";
        write_ppm(name, recv_count, np, Dwidth, Dheight, MandleBuffer);
        double time_average = 0;
        double temp = 0;
        for (int t = 0; t < send_count; t++)
        {
            temp = log_array[1][t] - log_array[0][t];
            printf("static, process, %d, worker, %d, time, %f\n", np, t, temp * 1000);
            time_average += temp;
        }
        printf("static, process, %d, average_time, %f\n", np, time_average * 1000 / (float)send_count);
        printf("Main Exection time: %f", (log_array[1][send_count] - log_array[0][send_count]) * 1000);
        free(MandleBuffer);
        free(log_array);
        MPI_Finalize();
        exit(0);
    }

    else
    {
        int job_number;
        int width_n;
        while (1)
        {
            MPI_Recv(&job_number, Alloc, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            WorkerBuffer[0] = job_number;
            width_n = job_number * Nrect;
            if (status.MPI_TAG == KILL_TAG)
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
                MPI_Send(WorkerBuffer, Nrect + 2, MPI_INT, 0, DATA_TAG, MPI_COMM_WORLD);
            }
            MPI_Send(WorkerBuffer, Nrect + 2, MPI_INT, 0, FIN_TAG, MPI_COMM_WORLD);
        }
    }
}