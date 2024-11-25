#include <exception>

void call()
{
}

void call_wrapper()
{
    call();
}

void check_condition(bool condition)
{
    if (condition)
    {
        throw std::exception();
    }
}

void my_fn(bool outer, bool condition)
{

    if (condition)
    {
        throw std::exception();
    }

    check_condition(condition);

    call();

    call_wrapper();
}