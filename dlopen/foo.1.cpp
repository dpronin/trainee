#include "foo.h"

#include <iostream>

#define VERSION 1

namespace foo::v1 {
    void bar() {
        std::cout << "version: " << VERSION << std::endl;
    }
}

namespace {
    call_table_v1 const ct = {
        .bar = foo::v1::bar,
    };

    call_table_description const ctd = {
        .version = VERSION,
        .call_table = &ct,
    };
}

call_table_description get_call_table_description(void)
{
    return ctd;
}
