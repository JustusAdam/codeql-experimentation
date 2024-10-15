#include <vector>

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
    //v.push_back(source());
    v[0] = source();
    auto elem = v[0];
    return target(elem);
}
