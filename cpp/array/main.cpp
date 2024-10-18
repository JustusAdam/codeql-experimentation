#include <vector>

__attribute__((noinline))
int source()
{
    return 2;
}

__attribute__((noinline)) int target(int source)
{
    return source;
}

int main(int argv, char **argc)
{
    // Even works with this pointer reassignment

    int arr[1] = { 0 };
    arr[0] = source();

    int* arrp = arr;
    return target(arrp[0]);
}
