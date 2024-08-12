#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <filesystem>
#include <iostream>
#include <memory>
#include <print>
#include <string_view>

#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/prov_ssl.h>
#include <openssl/ssl.h>
#include <openssl/types.h>

using namespace std::string_view_literals;

constexpr auto kServicePortDefault{"3333"sv};

namespace {

[[noreturn]] void show_help(std::string_view program) {
  auto const help{
      R"HELP(
-p, --port       Port of the service to bind onto

--crt            Path to the certifacte of the service
--pkey           Path to the private key of the service

-h, --help       Show this help page
    )HELP"sv,
  };
  std::println(std::cout, "{}:", program);
  std::println(std::cout, "{}", help);
  exit(EXIT_SUCCESS);
}

} // namespace

int main(int argc, char const *argv[]) {
  std::string_view port{kServicePortDefault};
  std::filesystem::path crt{".crtkey/server.crt"};
  std::filesystem::path pkey{".crtkey/server.key"};

  auto const first{argv};
  auto const last{argv + argc};
  for (auto arg_it{first}; last != arg_it; ++arg_it) {
    if (!(strcmp(*arg_it, "-p") && strcmp(*arg_it, "--port")) &&
        (arg_it + 1) != last) {
      port = *(++arg_it);
    } else if (!strcmp(*arg_it, "--crt") && (arg_it + 1) != last) {
      crt = *(++arg_it);
    } else if (!strcmp(*arg_it, "--pkey") && (arg_it + 1) != last) {
      pkey = *(++arg_it);
    } else if (!(strcmp(*arg_it, "-h") && strcmp(*arg_it, "--help"))) {
      show_help(argv[0]);
      /* must not be here */
      std::abort();
    }
  }

  auto ctx{
      std::unique_ptr<SSL_CTX, void (*)(SSL_CTX *)>{
          SSL_CTX_new(TLS_server_method()),
          SSL_CTX_free,
      },
  };

  if (!ctx) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Failed to create server SSL_CTX");
    return EXIT_FAILURE;
  }

  if (!SSL_CTX_set_min_proto_version(ctx.get(), TLS1_3_VERSION)) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr,
                 "Failed to set the minimum TLSv1.3 protocol version");
    return EXIT_FAILURE;
  }

  uint64_t const opts{
      SSL_OP_IGNORE_UNEXPECTED_EOF | SSL_OP_NO_RENEGOTIATION |
          SSL_OP_CIPHER_SERVER_PREFERENCE,
  };

  SSL_CTX_set_options(ctx.get(), opts);

  if (!(SSL_CTX_use_certificate_file(ctx.get(), crt.c_str(), SSL_FILETYPE_PEM) >
        0)) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Failed to load the server certificate chain file");
    return EXIT_FAILURE;
  }

  if (!(SSL_CTX_use_PrivateKey_file(ctx.get(), pkey.c_str(), SSL_FILETYPE_PEM) >
        0)) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr,
                 "Error loading the server private key file, possible "
                 "key/cert mismatch???");
    return EXIT_FAILURE;
  }

  SSL_CTX_set_verify(ctx.get(), SSL_VERIFY_NONE, nullptr);

  assert(!port.empty());

  auto acceptor_bio{
      std::unique_ptr<BIO, int (*)(BIO *)>{
          BIO_new_accept(port.data()),
          BIO_free,
      },
  };

  ERR_print_errors_fp(stderr);

  if (!acceptor_bio) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Error creating acceptor bio");
    return EXIT_FAILURE;
  }

  BIO_set_bind_mode(acceptor_bio.get(), BIO_BIND_REUSEADDR);
  if (!(BIO_do_accept(acceptor_bio.get()) > 0)) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Error setting up acceptor socket");
    return EXIT_FAILURE;
  }

  while (true) {
    ERR_clear_error();

    if (!(BIO_do_accept(acceptor_bio.get()) > 0))
      continue;

    auto client_bio{
        std::unique_ptr<BIO, int (*)(BIO *)>{
            BIO_pop(acceptor_bio.get()),
            BIO_free,
        },
    };

    std::println(std::cout, "New client connection accepted from");

    auto client_ssl{
        std::unique_ptr<SSL, void (*)(SSL *)>{SSL_new(ctx.get()), SSL_free},
    };

    if (!client_ssl) {
      ERR_print_errors_fp(stderr);
      std::println(std::cerr, "Error creating SSL handle for new connection");
      continue;
    }

    SSL_set_bio(client_ssl.get(), client_bio.get(), client_bio.get());
    client_bio.release();

    if (!(SSL_accept(client_ssl.get()) > 0)) {
      ERR_print_errors_fp(stderr);
      std::println(std::cerr, "Error performing SSL handshake with client");
      continue;
    }

    std::array<char, 1024> buf;
    for (size_t nread{0};
         SSL_read_ex(client_ssl.get(), buf.data(), buf.size(), &nread);
         nread = 0) {

      std::println(std::cerr,
                   "Received message '{}' from the client. Echoing...",
                   std::string_view{buf.data(), nread});

      if (size_t nwritten{0};
          SSL_write_ex(client_ssl.get(), buf.data(), nread, &nwritten) &&
          nwritten == nread) {
        continue;
      }

      std::println(std::cerr, "Error echoing client input");

      break;
    }

    std::println(std::cerr, "Client connection closed");
  }

  return EXIT_SUCCESS;
}
