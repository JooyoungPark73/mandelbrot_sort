#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include "utils.h"

int main(int argc, char *argv[])
{
    const int real_max = 2;
    const int real_min = -2;
    const int imag_max = 2;
    const int imag_min = -2;
    const float Dheight = 400;
    const float Dwidth = 400;
    const float scale_real = (real_max - real_min) / Dwidth;
    const float scale_imag = (imag_max - imag_min) / Dheight;
    float complex c;
    int *WorkerBuffer = (int *)malloc(12 * sizeof(int));

    // float x = atof(argv[1]);
    // float y = atof(argv[2]);
    // float c_real = real_min + (x * scale_real);
    // float c_imag = imag_min + (y * scale_imag);
    // c = CMPLXF(c_real, c_imag);

    // int color = calc_pixel(c);
    // printf("input: %.3f+j%.3f, coord: %.3f+j%.3f, pixel_calc: %d \n", x, y, creal(c), cimag(c), color);
    int width_n = 8;
    WorkerBuffer[0] = width_n;
    for (int x = 0; x < Dheight; x++)
    {
        WorkerBuffer[1] = x;
        for (int y = width_n; y < (width_n + 10); y++)
        {
            float complex c;
            float c_real = real_min + (x * scale_real);
            float c_imag = imag_min + (y * scale_imag);
            c = CMPLXF(c_real, c_imag);
            int color = calc_pixel(c);
            WorkerBuffer[y - width_n + 2] = color;
        }
        printf("Width: %d, ", WorkerBuffer[0]);
        printf("Height: %d, Val: ", WorkerBuffer[1]);
        for (int i = 2; i < 12; i++)
        {
            printf("%d", WorkerBuffer[i]);
        }
        printf("\n");
    }
    return 0;
}