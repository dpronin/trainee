#include "foo.h"

#include <iostream>

#define VERSION 2

namespace foo::v2 {
    void bar(int param) {
        std::cout << "version: " << VERSION << ", param: " << param << std::endl;
    }
}

namespace {
    call_table_v2 const ct = {
        .bar = foo::v2::bar,
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
