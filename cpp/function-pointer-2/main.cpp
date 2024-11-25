
int source()
{
    return 2;
}

int a_function()
{
    return source();
}

int target(int source)
{
    return source;
}

void set(int (**ptr)(), int (*ptr2)())
{
    *ptr = ptr2;
}

int main(int argv, char **argc)
{
    int (*fptr)();

    set(&fptr, a_function);

    target(fptr());

    fptr = a_function;

    target(fptr());

    return 0;
}
