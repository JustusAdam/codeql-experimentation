
int source()
{
    return 2;
}

int target(int source)
{
    return source;
}

int target_wrapper(int source)
{
    return target(source);
}

void apply(int (*f)(int), int argument)
{
    f(argument);
}

int main(int argv, char **argc)
{
    apply(target_wrapper, source());

    return 0;
}
