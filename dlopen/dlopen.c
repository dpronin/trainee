#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

#include "foo.h"

int main(int argc, char **argv) {
  int const version = atoi(argv[1]);

  void *lib;
  switch (version) {
  case 1:
    lib = dlopen("./libfoo.1.so", RTLD_NOW);
    break;
  case 2:
    lib = dlopen("./libfoo.2.so", RTLD_NOW);
    break;
  default:
    fprintf(stderr, "unsupported version %i of library\n", version);
    return EXIT_FAILURE;
  }

  if (!lib) {
    perror("could not open library");
    return EXIT_FAILURE;
  }

  typeof(get_call_table_description) *call_table_description_getter_f =
      dlsym(lib, "get_call_table_description");

  struct call_table_description const ctd = call_table_description_getter_f();
  assert(ctd.version == version);

  switch (ctd.version) {
  case 1: {
    struct call_table_v1 const *pct_v1 = ctd.call_table;
    pct_v1->bar();
  } break;
  case 2: {
    struct call_table_v2 const *pct_v2 = ctd.call_table;
    pct_v2->bar(42);
  } break;
  default:
    assert(0);
  }

  dlclose(lib);

  return 0;
}
