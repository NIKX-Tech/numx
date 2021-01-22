#include "integral.h"

double trapezium(double from, double to, double n, double (*func)())
{
    double h = (to - from) / n;
    double sum = func(from) + func(to);
    int i;
    for (i = 1; i < n; i++)
        sum += 2.0 * func(from + i * h);
    return h * sum / 2.0;
}