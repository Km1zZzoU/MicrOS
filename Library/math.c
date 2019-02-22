#include "math.h"

//Trigonometric functions

double cos(double x)
{
    __asm__ (
        "fldl  %1 \n" \
        "fcos \n" \
        "fstpl %0"
        : "=m"(x): "m"(x));
    return x;
}

float cosf(float x)
{
    __asm__ (
        "fld  %1 \n" \
        "fcos \n" \
        "fstp %0"
        : "=m"(x): "m"(x));
    return x;
}

double sin(double x)
{
    __asm__ (
        "fldl  %1 \n" \
        "fsin \n" \
        "fstpl %0"
        : "=m"(x): "m"(x));
    return x;
}

float sinf(float x)
{
    __asm__ (
        "fld  %1 \n" \
        " fsin \n" \
        "fstp %0"
        : "=m"(x): "m"(x));
    return x;
}

double tan(double x)
{
    __asm__ (
        "fldl  %1 \n" \
        "fptan \n" \
        "fstpl %0 \n" \
        "fstpl %0"
        : "=m"(x): "m"(x));
    return x;
}

float tanf(float x)
{
    __asm__ (
        "fld  %1 \n" \
        "fptan \n" \
        "fstp %0 \n" \
        "fstp %0"
        : "=m"(x): "m"(x));
    return x;
}

double acos(double x)
{
    double two = 2;
    double asinX = asin(x);
    __asm__ (
        "fldl %1 \n" \
        "fldpi \n" \
        "fdivp \n" \
        "fldl %2 \n" \
        "fsubrp \n" \
        "fstpl %0"
        : "=m"(x): "m"(two), "m"(asinX));
    return x;
}

float acosf(float x)
{
    float two = 2;
    float asinX = asinf(x);
    __asm__ (
        "fld %1 \n" \
        "fldpi \n" \
        "fdivp \n" \
        "fld %2 \n" \
        "fsubrp \n" \
        "fstp %0"
        : "=m"(x): "m"(two), "m"(asinX));
    return x;
}

double asin(double x)
{
    __asm__ (
        "fldl  %1 \n" \
        "fldl  %1 \n" \
        "fmulp \n" \
        "fld1 \n" \
        "fsubp \n" \
        "fsqrt \n" \
        "fldl  %1 \n" \
        "fdivp \n" \
        "fld1 \n" \
        "fpatan \n" \
        "fstpl %0"
        : "=m"(x): "m"(x));
    return x;
}

float asinf(float x)
{
    __asm__ (
        "fld  %1 \n" \
        "fld  %1 \n" \
        "fmulp \n" \
        "fld1 \n" \
        "fsubp \n" \
        "fsqrt \n" \
        "fld  %1 \n" \
        "fdivp \n" \
        "fld1 \n" \
        "fpatan \n" \
        "fstp %0"
        : "=m"(x): "m"(x));
    return x;
}

double atan(double x)
{
    __asm__ (
        "fldl  %1 \n" \
        "fld1 \n" \
        "fpatan \n" \
        "fstpl %0 \n"
        : "=m"(x): "m"(x));
    return x;
}

float atanf(float x)
{
    __asm__ (
        "fld  %1 \n" \
        "fld1 \n" \
        "fpatan \n" \
        "fstp %0 \n"
        : "=m"(x): "m"(x));
    return x;
}

double atan2(double x, double y)
{
    __asm__ (
        "fldl %1 \n" \
        "fldl %2 \n" \
        "fpatan \n" \
        "fstpl %0 \n"
        : "=m"(x): "m"(x), "m"(y));
    return x;
}

float atan2f(float x, float y)
{
    __asm__ (
        "fld %1 \n" \
        "fld %2 \n" \
        "fpatan \n" \
        "fstp %0 \n"
        : "=m"(x): "m"(x), "m"(y));
    return x;
}

// Hyperbolic functions

double acosh(double x)
{
    return log(x + sqrt(x * x - 1));
}

float acoshf(float x)
{
    return logf(x + sqrtf(x * x - 1));
}

double asinh(double x)
{
    return log(x + sqrt(x * x + 1));
}

float asinhf(float x)
{
    return logf(x + sqrtf(x * x + 1));
}

double atanh(double x)
{
    return 0.5 * log((1 + x)/(1 - x));
}

float atanhf(float x)
{
    return 0.5 * logf((1 + x)/(1 - x));
}

// Exponential and logarithmic functions

double log(double x)
{
    return log2(x)/log2(M_E);
}

float logf(float x)
{
    return log2f(x)/log2f(M_E);
}

double log10(double x)
{
    return log2(x)/log2(10.);
}

float log10f(float x)
{
    return log2f(x)/log2f(10.);
}

double log2(double x)
{
    __asm__ (
        "fld1 \n" \
        "fldl %1 \n" \
        "fyl2x  \n" \
        "fstpl %0 \n"
        : "=m"(x): "m"(x));
    return x;
}

float log2f(float x)
{
    __asm__ (
        "fld1 \n" \
        "fld %1 \n" \
        "fyl2x \n" \
        "fstp %0 \n"
        : "=m"(x): "m"(x));
    return x;
}

// Power functions 

double sqrt(double x)
{
    __asm__ (
        "fldl  %1 \n" \
        "fsqrt \n fstpl %0"
        : "=m"(x): "m"(x));
    return x;  
}

float sqrtf(float x)
{
    __asm__ (
        "fld  %1 \n" \
        "fsqrt \n" \
        "fstp %0"
        : "=m"(x): "m"(x));
    return x;
}


//Nearest integer floating-point operations

double round(double x)
{
    __asm__ (
        "fld  %1 \n" \
        "frndint \n" \
        "fstpl %0"
        : "=m"(x): "m"(x));
    return x;
}

float roundf(float x)
{
    __asm__ (
        "fld  %1 \n" \
        "frndint \n" \
        "fstp %0"
        : "=m"(x): "m"(x));
    return x;
}

// Other functions

double fabs(double x)
{
    __asm__ (
        "fldl  %1 \n" \
        "fabs \n fstpl %0"
        : "=m"(x): "m"(x));
    return x;
}

float fabsf(float x)
{
    __asm__ (
        "fld  %1 \n" \
        "fabs \n fstp %0"
        : "=m"(x): "m"(x));
    return x;
}