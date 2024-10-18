#include <string>

__attribute__((noinline))
int source()
{
    return 2;
}

__attribute__((noinline)) 
int target(std::string source)
{
    return source.size();
}

int main(int argv, char **argc)
{
    // In this case the call `source()` is not present as an expression in
    // codeql
    std::string s = { static_cast<char>(source())};
    return target(s);
}
