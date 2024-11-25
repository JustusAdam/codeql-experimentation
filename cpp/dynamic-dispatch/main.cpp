int source()
{
    return 2;
}

int target(int source)
{
    return source;
}

class Base
{
public:
    virtual int f() = 0;
};

class A : public Base
{
    int f() { return source(); }
};

struct Container
{
    Base *b;
};

int main(int argv, char **argc)
{
    Base *b = new A;
    target(b->f());

    Container c;
    c.b = new A;
    target(c.b->f());
    return 0;
}