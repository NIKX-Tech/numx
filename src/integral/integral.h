#ifndef _INTEGRAL_H
#define _INTEGRAL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum
{
    RECTANGLE_LEFT,
    RECTANGLE_RIGHT,
    RECTANGLE_MID,
    TRAPEZIUM,
    SIMPSON
} integral_type;

double leftRect(double from, double to, double n, double (*func)());
double rightRect(double from, double to, double n, double (*func)());
double midRect(double from, double to, double n, double (*func)());
double trapezium(double from, double to, double n, double (*func)());
double simpson(double from, double to, double n, double (*func)());

inline double calc(integral_type type, double from, double to, double n, double (*func)())
{
    switch (type)
    {
    case RECTANGLE_LEFT:
        return leftRect(from, to, n, *func);
        break;
    case RECTANGLE_RIGHT:
        return rightRect(from, to, n, *func);
        break;
    case RECTANGLE_MID:
        return midRect(from, to, n, *func);
        break;
    case TRAPEZIUM:
        return trapezium(from, to, n, *func);
        break;
    case SIMPSON:
        return simpson(from, to, n, *func);
        break;
    }
}

#endif