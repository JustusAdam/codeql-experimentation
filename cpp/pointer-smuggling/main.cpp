#include <iostream>

int source()
{
    return 2;
}

int target(int source)
{
    return source;
}

int* return_ptr(int* p, int* q) {
    return p;
}

int main(int argv, char **argc) {
    int a, b;
    int *c = return_ptr(&a, &b);
    *c = source();
    target(a);
    return 0;
}