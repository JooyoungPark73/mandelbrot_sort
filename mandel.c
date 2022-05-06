#include <complex.h>
#include "mandel_l.h"

int cal_pixel(complex c)
{
    int count, max_iter;
    float complex z;
    float tempr, tempi, lengthsq;
    max_iter = 256;
    z = CMPLX(0, 0);
    count = 0;
    do
    {
        tempr = creal(z) * creal(z) - cimag(z) * cimag(z) + creal(c);
        tempi = 2 * creal(z) * cimag(z) + cimag(c);
        z = CMPLX(tempr, tempi);
        lengthsq = creal(z) * creal(z) + cimag(z) * cimag(z);
        count++;
    } while ((lengthsq < 4.0) && (count < max_iter));
    return count;
}

int cal_plane(int dWidth, int dHeight, int **display)
{
    for (int x = 0; x < dWidth; x++)
    {
        for (int y = 0; y < dHeight; y++)
        {
            display[y][x] = cal_pixel(CMPLX(x, y));
        }
    }
}