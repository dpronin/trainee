#include <clang-c/Index.h>

#include <cstdlib>

#include <iostream>

/* clang-format off */

/*
 * Example (works out if you have got llvm of exactly 18th major version):
 *
 * $ LIBCLANG_TIMING=1 ./clang_complete ../project.cpp 9 5 --driver-mode=g++
 * -I/usr/lib/llvm/18/include -g -c -std=gnu++23
 * -resource-dir=/usr/lib/llvm/18/bin/../../../../lib/clang/18 --
 *
 * Output:
 *
 * Parsing ../project.cpp:   0.0012 (100.0%)   0.0012 (100.0%)   0.0024 (100.0%)   0.0024 (100.0%)
 * Reparsing ../project.cpp:   0.0009 (100.0%)   0.0008 (100.0%)   0.0016 (100.0%)   0.0016 (100.0%)
 * Code completion @ ../project.cpp:9:5:   0.0020 (100.0%)   0.0010 (100.0%)   0.0029 (100.0%)   0.0029 (100.0%)
 * bar
 * Foo
 * ~Foo
 * operator=
 * operator=
 *
 */

/* clang-format on */

[[noreturn]] void help_show(char const *program, int ec = EXIT_SUCCESS) {
  std::cout << program << " file.cpp line colum [clang args...]'" << std::endl;
  _Exit(ec);
}

int main(int argc, char **argv) {
  if (argc < 4)
    help_show(argv[0], EXIT_FAILURE);

  auto idx{clang_createIndex(1, 0)};
  if (!idx) {
    std::cerr << "createIndex failed" << std::endl;
    return EXIT_FAILURE;
  }

  auto u{CXTranslationUnit{}};
  auto const e{
      clang_parseTranslationUnit2(idx, argv[1], argv + 4, argc - 4, 0, 0,
                                  CXTranslationUnit_KeepGoing |
                                      CXTranslationUnit_PrecompiledPreamble,
                                  &u),
  };
  if (e) {
    std::cerr << "parseTranslationUnit failed " << e << std::endl;
    return 2;
  }

  auto ec{clang_reparseTranslationUnit(u, 0, 0, CXTranslationUnit_KeepGoing)};
  if (ec) {
    std::cerr << "reparsing failed with errcode " << ec << "\n";
    return 3;
  }

  auto const line{std::strtol(argv[2], 0, 10)};
  auto const column{std::strtol(argv[3], 0, 10)};
  auto *res = clang_codeCompleteAt(u, argv[1], line, column, 0, 0, 0);
  if (!res) {
    std::cerr << "Could not complete" << std::endl;
    return 2;
  }

  for (unsigned i = 0; i < clang_codeCompleteGetNumDiagnostics(res); i++) {
    auto const &diag{clang_codeCompleteGetDiagnostic(res, i)};
    std::cout << clang_getCString(clang_getDiagnosticSpelling(diag)) << '\n';
  }

  for (unsigned i = 0; i < res->NumResults; i++) {
    auto const &completion_str = res->Results[i].CompletionString;

    for (unsigned j = 0; j < clang_getNumCompletionChunks(completion_str);
         j++) {
      if (CXCompletionChunk_TypedText ==
          clang_getCompletionChunkKind(completion_str, j)) {
        std::cout << clang_getCString(
                         clang_getCompletionChunkText(completion_str, j))
                  << '\n';
      }
    }
  }

  clang_disposeCodeCompleteResults(res);

  return 0;
}
