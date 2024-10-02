
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

void process(int *input) {
    *input += 1;
}

void process2(int *input) {
    process(input);
}

void process3(int *input) {
    process2(input);
}

void process4(int *input) {
    process3(input);
}

void process5(int *input) {
    process4(input);
}

void process6(int *input) {
    process5(input);
}

void process7(int *input) {
    process6(input);
}

void attach_source(int *input) {
    *input = source();
}

int main(int argv, char **argc)
{
    int src = 0;
    attach_source(&src);
    process7(&src);

    int other = 100;

    process7(&other);

    target(other);

    return 0;
}
