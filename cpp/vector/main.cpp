#include <vector>

__attribute__((noinline)) int source()
{
    return 2;
}

__attribute__((noinline)) int target(int source)
{
    return source;
}

template <typename T>
__attribute__((noinline)) int target(std::vector<T> &source)
{
    return source.size();
}

int main(int argv, char **argc)
{
    int result = 0;

    // Strangely these working configs only work with the `AllFlows` config,
    // even if the `isSink` and `isSource` predicates are used to select the
    // dataflow nodes we test in the query.

    // latest run is with codeql 2.19.3 and on ubuntu where presumably we use
    // gcc (version 11.4.0)

    // index version with for loop
    //
    // works

    std::vector<int> v0 = {0};
    v0[0] = source();
    int ret = 0;
    for (auto elem : v0)
    {
        ret += target(elem);
    }
    result += ret;

    // index version with index retrieval
    //
    // works

    std::vector<int> v1 = {0};
    v1[0] = source();
    result += target(v1[0]);

    // push back with indexed retrieval
    //
    // Works

    std::vector<int> v2;
    v2.push_back(source());
    auto elem2 = v2[0];
    result += target(elem2);

    // indexing with "at" retrieval
    //
    // works

    std::vector<int> v3;
    v3[0] = source();
    auto elem3 = v3.at(0);
    result += target(elem3);

    // Push back with loop.
    //
    // Works.

    std::vector<int> v4;
    v4.push_back(source());
    int ret1 = 0;
    for (auto elem : v4)
    {
        ret1 += target(elem);
    }
    result += ret1;

    std::vector<int> v5;
    v5.push_back(source());
    result += target(v5);

    // Arrays do work, even with a reassigned pointer

    int arr[1] = {0};
    arr[0] = source();

    int *arrp = arr;
    result += target(arrp[0]);

    return result;
}
