#include <vector>

int source()
{
    return 42;
}

void target(std::vector<int> &vec)
{
}

void some_main()
{
    std::vector<int> vec;
    vec.push_back(source());
    target(vec);
}