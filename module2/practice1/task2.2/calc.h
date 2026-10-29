#ifndef CALC_H
#define CALC_H

#define CALC_SUCCESS 0
#define CALC_ERR_DIV_ZERO -1

double calc_add(double a, double b);
double calc_sub(double a, double b);
double calc_mul(double a, double b);

// Возвращает CALC_SUCCESS и записывает результат по указателю, 
// либо возвращает CALC_ERR_DIV_ZERO
int calc_div(double a, double b, double* result);

#endif // CALC_H