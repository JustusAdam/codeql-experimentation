
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

int target2(int input)
{
    return input;
}

int main(int argv, char **argc)
{
    switch (source()) {
        case true:
            target(1);
            break;
        default:
            target2(2);
    }

    return 0;
}
