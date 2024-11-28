#include <unordered_map>

int source()
{
    return 42;
}

template <typename T>
void sink(T &src) {}

void test()
{
    std::unordered_map<int, int> map;
    map[0] = source();
    sink(map[0]);
    sink(map);
}