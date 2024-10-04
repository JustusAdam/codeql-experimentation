#include <thread>

int source()
{
    return 2;
}

void a_function(int * result)
{
    *result = source();
}

int target(int source)
{
    return source;
}

int main(int argv, char **argc)
{
    int outer_result;
    std::thread t(a_function, &outer_result);
    t.join();
    target(outer_result);

    return 0;
}
