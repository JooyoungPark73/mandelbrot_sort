#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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

    /* for pretty grey color output */
    count = ceil(max_iter * sqrt((double)count / (double)max_iter));
    return count;
}

int write_ppm(char *mode, int iter, int np, int Dwidth, int Dheight, int **output)
{
    int i, j;
    char directory[1024];
    char name[1024];
    struct stat st = {0};

    snprintf(directory, 1024, "./images/%s/%dw", mode, np - 1);
    snprintf(name, 1024, "./images/%s/%dw/mandelbort_%d.ppm", mode, np - 1, iter);

    if (stat(directory, &st) == -1)
    {
        mkdir(directory, 0777);
    }

    FILE *fp = fopen(name, "wb");
    char *comment = "# ";
    fprintf(fp, "P6\n %s\n %d\n %d\n 255\n", comment, Dwidth, Dheight);
    for (j = 0; j < Dheight; ++j)
    {
        for (i = 0; i < Dwidth; ++i)
        {
            static unsigned char color[3];
            if (output[j][i] < 0)
            {
                color[0] = 0;
                color[1] = 255;
                color[2] = 0;
            }
            else
            {
                color[0] = output[j][i];
                color[1] = output[j][i];
                color[2] = output[j][i];
            }
            fwrite(color, 1, 3, fp);
        }
    }
    fclose(fp);
    return 0;
}
