#include <openssl/evp.h>

#include <cstdio>
#include <memory>
#include <string_view>
#include <vector>

constexpr size_t kChunkSz = 32;

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    std::fprintf(stdout, "digest algorithm specification required: md5 | "
                         "sha1 | sha224 | sha256 | sha384 | sha512 | "
                         "sha3-224 | sha3-256 | sha3-384 | sha3-512\n");
    return 1;
  }

  char const *algo_s = argv[1];

  struct evp_md_ctx_deleter {
    void operator()(EVP_MD_CTX *p_ctx) noexcept { EVP_MD_CTX_free(p_ctx); }
  };
  auto ctx = std::unique_ptr<EVP_MD_CTX, evp_md_ctx_deleter>{EVP_MD_CTX_new()};

  EVP_MD const *algo_md = nullptr;

  using namespace std::string_view_literals;
  if ("md5"sv == algo_s) {
    algo_md = EVP_md5();
  } else if ("sha1"sv == algo_s) {
    algo_md = EVP_sha1();
  } else if ("sha224"sv == algo_s) {
    algo_md = EVP_sha224();
  } else if ("sha256"sv == algo_s) {
    algo_md = EVP_sha256();
  } else if ("sha384"sv == algo_s) {
    algo_md = EVP_sha384();
  } else if ("sha512"sv == algo_s) {
    algo_md = EVP_sha512();
  } else if ("sha3-224"sv == algo_s) {
    algo_md = EVP_sha3_224();
  } else if ("sha3-256"sv == algo_s) {
    algo_md = EVP_sha3_256();
  } else if ("sha3-384"sv == algo_s) {
    algo_md = EVP_sha3_384();
  } else if ("sha3-512"sv == algo_s) {
    algo_md = EVP_sha3_512();
  } else {
    std::fprintf(stderr, "unsupported digest algorithm '%s' given\n", algo_s);
    return 1;
  }

  int const md_size = EVP_MD_size(algo_md);
  if (md_size <= 0) {
    std::fprintf(stderr, "invalid digest size for '%s'\n", algo_s);
    return 1;
  }
  std::vector<unsigned char> digest(static_cast<size_t>(md_size));

  if (1 != EVP_DigestInit_ex2(ctx.get(), algo_md, nullptr)) {
    std::fprintf(stdout, "digest '%s' init failed\n", algo_s);
    return 1;
  }

  char buf[kChunkSz];
  for (size_t sz{0}; (sz = std::fread(buf, 1, sizeof(buf), stdin)) > 0;) {
    if (1 != EVP_DigestUpdate(ctx.get(), buf, sz)) {
      std::fprintf(stdout, "digest '%s' update failed\n", algo_s);
      return 1;
    }
  }

  if (std::ferror(stdin) || !std::feof(stdin)) {
    std::fprintf(stderr, "input error\n");
    return 1;
  }

  if (unsigned int sz{0};
      1 != EVP_DigestFinal_ex(ctx.get(), digest.data(), &sz) ||
      digest.size() != sz) {
    std::fprintf(stderr, "digest '%s' generation error\n", algo_s);
    return 1;
  }

  auto to_hex = [](unsigned char c) {
    return 0 <= c && c <= 9 ? '0' + c : 'a' + c - 10;
  };

  for (unsigned char c : digest) {
    std::putc(to_hex(c >> 4), stdout);
    std::putc(to_hex(c & 0x0f), stdout);
  }

  return 0;
}
