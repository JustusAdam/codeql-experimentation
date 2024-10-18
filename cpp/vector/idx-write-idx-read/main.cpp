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
    std::vector<int> v = { 0 };
    v[0] = source();
    return target(v[0]);
}
