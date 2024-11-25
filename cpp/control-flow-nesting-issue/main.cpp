#include <exception>

void call()
{
}

void my_fn(bool outer, bool condition)
{

    if (condition)
    {
        throw std::exception();
    }

    if (outer)
    {
        if (condition)
        {
            throw std::exception();
        }
    }

    call();
}