#include <string>
#include <sstream>

__attribute__((noinline)) int source()
{
    return 2;
}

__attribute__((noinline)) int target(std::string source)
{
    return source.size();
}

int main(int argv, char **argc)
{
    std::stringstream ss;
    ss << source();
    return target(ss.str());
}
