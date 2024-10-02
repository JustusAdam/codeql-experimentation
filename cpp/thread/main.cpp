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
    int result;
    std::thread t(a_function, &result);
    t.join();
    target(result);

    return 0;
}
