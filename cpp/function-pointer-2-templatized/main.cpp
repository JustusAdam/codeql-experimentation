
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

template<typename T>
void set(T* ptr, T ptr2) {
    *ptr = ptr2;
}

int main(int argv, char **argc)
{
    int (*fptr)();

    // fptr = a_function;
    set(&fptr, a_function);

    target(fptr());

    return 0;
}
