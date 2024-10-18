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
    // codeql.
    //
    // Refactoring into separate variable bindings makes the call appear, but
    // the taint is lost at the `static_cast`.
    std::string s = { static_cast<char>(source())};
    return target(s);
}
