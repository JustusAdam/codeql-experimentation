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

class B : public Base
{
    int f() { return 1; }
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

    Base *b2 = new B();
    Base **b2_ptr = &b2;
    *b2_ptr = new A();
    target(b2->f());

    return 0;
}