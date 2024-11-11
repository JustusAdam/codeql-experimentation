#include <vector>

__attribute__((noinline))
int source()
{
    return 2;
}

__attribute__((noinline)) int target(int source)
{
    return source;
}

int main(int argv, char **argc)
{
    // Strangely these working configs only work with the `AllFlows` config,
    // even if the `isSink` and `isSource` predicates are used to select the
    // dataflow nodes we test in the query.

    // index version with for loop
    //
    // this one actually works

    // std::vector<int> v = { 0 };
    // v[0] = source();
    // int ret = 0;
    // for (auto elem : v)
    // {
    //     ret += target(elem);
    // }
    // return ret;

    // index version
    // 
    // Does not work

    std::vector<int> v = { 0 };
    v[0] = source();
    return target(v[0]);

    // // push back version
    // // 
    // // Does not work

    // std::vector<int> v;
    // v.push_back(source());
    // auto elem = v[0];
    // return target(elem);

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
}
