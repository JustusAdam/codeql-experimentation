// semmle-extractor-options: -std=c++17 --c++17 --clang
#include "../vector.h"

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
    int ret = 0;
    for (auto elem : v)
    {
        ret += target(elem);
    }
    return ret;
}
