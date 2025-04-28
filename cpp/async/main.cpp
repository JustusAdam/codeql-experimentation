#include <future>

int source()
{
    return 2;
}

int target(int source)
{
    return source;
}

void a_function(int result)
{
    target(result);
}

int main(int argv, char **argc)
{

    int outer_result = source();
    auto fut = std::async(a_function, outer_result);
    fut.get();
    return 0;
}
