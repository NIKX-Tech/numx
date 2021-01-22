#include "integral.h"

double leftRect(double from, double to, double n, double (*func)())
{
    double h = (to - from) / n;
    double sum = 0.0, x;
    for (x = from; x <= (to - h); x += h)
        sum += func(x);
    return h * sum;
}

double rightRect(double from, double to, double n, double (*func)())
{
    double h = (to - from) / n;
    double sum = 0.0, x;
    for (x = from; x <= (to - h); x += h)
        sum += func(x + h);
    return h * sum;
}

double midRect(double from, double to, double n, double (*func)())
{
    double h = (to - from) / n;
    double sum = 0.0, x;
    for (x = from; x <= (to - h); x += h)
        sum += func(x + h / 2.0);
    return h * sum;
}