#include "calc.h"

double calc_add(double a, double b) {
    return a + b;
}

double calc_sub(double a, double b) {
    return a - b;
}

double calc_mul(double a, double b) {
    return a * b;
}

int calc_div(double a, double b, double* result) {
    if (b == 0.0) {
        return CALC_ERR_DIV_ZERO;
    }
    *result = a / b;
    return CALC_SUCCESS;
}