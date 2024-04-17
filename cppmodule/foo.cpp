module;

#include <iostream>

export module foo;

export namespace foo {

void foo()
{
    std::cout << "Hello, world" << std::endl;
}

}
