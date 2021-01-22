#include <stdio.h>
#include "integral\integral.h"

double f1(double x)
{
    return x * x * x;
}

double f2(double x)
{
    return 1.0 / x;
}

double f3(double x)
{
    return x;
}

void printTestFunction(double from, double to, double n, double (*func)())
{
    printf("rectangularLeft:  %1f\n", calc(RECTANGLE_LEFT, from, to, n, *func));
    printf("rectangularRight:  %1f\n", calc(RECTANGLE_RIGHT, from, to, n, *func));
    printf("rectangularMid:  %1f\n", calc(RECTANGLE_MID, from, to, n, *func));
    printf("trapezium:  %1f\n", calc(TRAPEZIUM, from, to, n, *func));
    printf("simpsons:  %1f\n\n", calc(SIMPSONS, from, to, n, *func));
}

int main()
{
    printTestFunction(0.0, 1.0, 100.0, f1);
    printTestFunction(1.0, 100.0, 1000.0, f2);
    printTestFunction(0.0, 5000.0, 5000000.0, f3);
    printTestFunction(0.0, 6000.0, 6000000.0, f3);

    getchar();
}