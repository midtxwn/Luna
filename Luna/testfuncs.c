
// testfuncs.c (double version)
double func_void(void) { return 42.0; }
double func_one(double x) { return x + 1.0; }
double func_two(double x, double y) { return x + y; }
double fact(double x)
{
    for(unsigned long i = x-1; i > 0; --i)
        x *= i;
    return x;

}