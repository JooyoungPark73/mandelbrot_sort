#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include "util.h"

int calc_pixel(float complex c)
{
    int count, max_iter;
    float complex z;
    float tempr, tempi, lengthsq;
    max_iter = 256;
    z = CMPLXF(0, 0);
    count = 0;
    do
    {
        tempr = creal(z) * creal(z) - cimag(z) * cimag(z) + creal(c);
        tempi = 2 * creal(z) * cimag(z) + cimag(c);
        z = CMPLXF(tempr, tempi);
        lengthsq = creal(z) * creal(z) + cimag(z) * cimag(z);
        count++;
    } while ((lengthsq < 4.0) && (count < max_iter));
    return count;
}

int write_ppm(int Dwidth, int Dheight, int **output)
{
    int i, j;
    FILE *fp = fopen("mandelbort_static.ppm", "wb"); /* b - binary mode */
    (void)fprintf(fp, "P6\n%d %d\n255\n", Dwidth, Dheight);
    for (j = 0; j < Dheight; ++j)
    {
        for (i = 0; i < Dwidth; ++i)
        {
            static unsigned char color[3];
            color[0] = output[j][i]; /* red */
            color[1] = output[j][i]; /* green */
            color[2] = output[j][i]; /* blue */
            (void)fwrite(color, 1, 3, fp);
        }
    }
    (void)fclose(fp);
    return EXIT_SUCCESS;
}
