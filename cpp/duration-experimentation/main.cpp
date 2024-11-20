#include <chrono>

int testfn()
{
    auto since_epoch = std::chrono::system_clock::now().time_since_epoch();
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
    auto now = secs.count();
    return now;
}