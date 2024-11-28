int source()
{
    return 2;
}

int target(int source)
{
    return source;
}

int a_function()
{
    return source();
}

int b_function()
{
    return 0;
}

int main(int argv, char **argc)
{
    int (*fptr)();

    fptr = a_function;

    target(fptr());

    int (*fptr2)();
    int (**fptr2_ptr)() = &fptr2;
    fptr2 = b_function;
    *fptr2_ptr = a_function;

    target(fptr2());

    return 0;
}
