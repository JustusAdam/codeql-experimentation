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
    std::vector<int> v;
    v.push_back(source());
    auto elem = v[0];
    return target(elem);
}
