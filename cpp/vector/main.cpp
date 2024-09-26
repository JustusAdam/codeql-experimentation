#include <vector>

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
    std::vector<int> v;
    v.push_back(source());
    target(v[0]);

    return 0;
}
