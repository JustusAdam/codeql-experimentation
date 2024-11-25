int source()
{
    return 2;
}

int target(int source)
{
    return source;
}
int main(int argv, char **argc)
{
    int a;
    int *c = &a;
    *c = source();
    target(a);

    a = source();
    target(a);

    return 0;
}