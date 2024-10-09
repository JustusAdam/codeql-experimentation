
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

int main(int argv, char **argc)
{
    if (source()) {
        target(1);
    }

    return 0;
}
