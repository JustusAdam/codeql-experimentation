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
    // index version with for loop
    // 
    // this one actually works

    // std::vector<int> v = { 0 };
    // v[0] = source();
    // int ret = 0;
    // for (auto elem : v)
    // {
    //     ret += target(elem);
    // }
    // return ret;

    // index version
    // 
    // Does not work

    std::vector<int> v = { 0 };
    v[0] = source();
    return target(v[0]);

    // push back version
    // 
    // Does not work

    // std::vector<int> v;
    // v.push_back(source());
    // return target(v[0]);
}
