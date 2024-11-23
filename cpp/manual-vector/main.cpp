#include <cstring>
#include <iostream>

template <typename T>
class vec_iter
{
    T *__p;

public:
    vec_iter(T *p) : __p(p) {}

    bool operator!=(vec_iter<T> &other)
    {
        return __p != other.__p;
    }

    vec_iter<T> &operator++()
    {
        __p++;
        return *this;
    }

    T &operator*()
    {
        return *__p;
    }
};

template <typename T>
class vec
{
    T *__start;
    T *__end;
    uint64_t cap;

public:
    vec()
    {
        __start = 0;
        __end = __start;
        cap = 0;
    };

    void ensure_cap(uint64_t additional)
    {
        auto current = (uint64_t)(__end - __start);
        auto target = current + additional;
        if (target > cap)
        {
            auto new_cap = cap * 2;
            if (new_cap == 0)
            {
                new_cap = 4;
            }

            while (new_cap < target)
                new_cap *= 2;
            auto old_start = __start;
            __start = new T[new_cap];
            __end = __start + current;
            memcpy(__start, old_start, cap * sizeof(T));
            free(old_start);
            cap = new_cap;
        }
    }

    void push_back(const T &elem)
    {
        ensure_cap(1);
        *__end = elem;
        __end++;
    }

    vec_iter<T> begin()
    {
        return vec_iter(__start);
    }

    vec_iter<T> end()
    {
        return vec_iter(__end);
    }

    T &operator[](uint64_t n)
    {
        return *(__start + n);
    }
};

__attribute__((noinline)) int source()
{
    return 2;
}

__attribute__((noinline)) int target(int source)
{
    return source;
}

int main(int argv, char **argc)
{
    int result = 0;
    // For testing the vector works
    // vec<int> v = vec<int>();

    // v.push_back(1);
    // v.push_back(100);
    // v.push_back(50);
    // v.push_back(530);
    // v.push_back(509);
    // v.push_back(850);
    // v.push_back(750);
    // v.push_back(430);
    // v.push_back(220);

    // for (auto elem : v)
    // {
    //     std::cout << elem << std::endl;
    // }

    // return 0;

    // Strangely these working configs only work with the `AllFlows` config,
    // even if the `isSink` and `isSource` predicates are used to select the
    // dataflow nodes we test in the query.

    // index version with for loop
    //
    // this one actually works

    vec<int> v0 = vec<int>();
    v0[0] = source();
    int ret = 0;
    for (auto elem : v0)
    {
        ret += target(elem);
    }

    result += ret;

    // index version
    //
    // Does not work

    vec<int> v1 = vec<int>();
    v1[0] = source();
    result += target(v1[0]);

    // push back version
    //
    // Does not work

    vec<int> v2;
    v2.push_back(source());
    auto elem = v2[0];
    result += target(elem);

    // Does not work

    // std::vector<int> v;
    // v.push_back(source());
    // auto elem = v.at(0);
    // return target(elem);

    // Arrays do work, even with a reassigned

    // int arr[1] = { 0 };
    // arr[0] = source();

    // int* arrp = arr;
    // return target(arrp[0]);
    return result;
}
