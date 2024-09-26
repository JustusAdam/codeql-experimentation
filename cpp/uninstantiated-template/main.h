int source()
{
    return 2;
}

int target(int source)
{
    return source;
}

template<typename T>
void entry() {
    target(source());
}

// void entry() {
//     target(source());
// }