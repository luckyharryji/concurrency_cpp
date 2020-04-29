#include <iostream>

int * foo()
{
    int a = 5;
    return &a;
}

int main()
{
    int* p = foo();
    std::cout << *p;
    *p = 8;
    std::cout << *p;
}
