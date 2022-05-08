#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <string.h>
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

int write_ppm(char *mode, int iter, int Dwidth, int Dheight, int **output)
{
    int i, j;
    char directory[1024];
    snprintf(directory, 1024, "./images/%s/mandelbort_%d.ppm", mode, iter);
    FILE *fp = fopen(directory, "wb");
    char *comment = "# ";
    fprintf(fp, "P6\n %s\n %d\n %d\n 255\n", comment, Dwidth, Dheight);
    for (j = 0; j < Dheight; ++j)
    {
        for (i = 0; i < Dwidth; ++i)
        {
            static unsigned char color[3];
            // if (output[j][i] < 86)
            // {
            //     color[0] = 0;
            //     color[1] = output[j][i];
            //     color[2] = 255 - output[j][i];
            // }
            // if (output[j][i] >= 86 && output[j][i] < 171)
            // {
            //     color[0] = 255 - output[j][i];
            //     color[1] = 0;
            //     color[2] = output[j][i];
            // }
            // if (output[j][i] >= 171)
            // {
            //     color[0] = output[j][i];
            //     color[1] = 255 - output[j][i];
            //     color[2] = 0;
            // }
            color[0] = output[j][i];
            color[1] = output[j][i];
            color[2] = output[j][i];
            fwrite(color, 1, 3, fp);
        }
    }
    fclose(fp);
    return 0;
}
