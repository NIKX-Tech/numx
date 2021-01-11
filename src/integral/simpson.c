#include "integral.h"

double simpson(double from, double to, double n, double (*func)())
{
    double h = (to - from) / n;
    double sum1 = 0.0;
    double sum2 = 0.0;
    int i;

    double x;

    for (i = 0; i < n; i++)
        sum1 += func(from + h * i + h / 2.0);

    for (i = 1; i < n; i++)
        sum2 += func(from + h * i);

    return h / 6.0 * (func(from) + func(to) + 4.0 * sum1 + 2.0 * sum2);
}