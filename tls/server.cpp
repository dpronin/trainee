#include <cassert>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <print>

#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/prov_ssl.h>
#include <openssl/ssl.h>

int main(int argc, char const *argv[]) {
  std::string_view hostport;

  if (argc > 1)
    hostport = argv[1];

  if (hostport.empty())
    hostport = "localhost:8000";

  auto ctx{
      std::unique_ptr<SSL_CTX, void (*)(SSL_CTX *)>{
          SSL_CTX_new(TLS_server_method()),
          SSL_CTX_free,
      },
  };

  if (!ctx) {
    ERR_print_errors_fp(stderr);
    println(std::cerr, "Failed to create server SSL_CTX");
    return EXIT_FAILURE;
  }

  if (!SSL_CTX_set_min_proto_version(ctx.get(), TLS1_3_VERSION)) {
    ERR_print_errors_fp(stderr);
    println(std::cerr, "Failed to set the minimum TLS protocol version");
    return EXIT_FAILURE;
  }

  uint64_t const opts{
      SSL_OP_IGNORE_UNEXPECTED_EOF | SSL_OP_NO_RENEGOTIATION |
          SSL_OP_CIPHER_SERVER_PREFERENCE,
  };

  SSL_CTX_set_options(ctx.get(), opts);

  if (!(SSL_CTX_use_certificate_chain_file(ctx.get(), "server.pem") > 0)) {
    ERR_print_errors_fp(stderr);
    println(std::cerr, "Failed to load the server certificate chain file");
    return EXIT_FAILURE;
  }

  if (!(SSL_CTX_use_PrivateKey_file(ctx.get(), "server.key", SSL_FILETYPE_PEM) >
        0)) {
    ERR_print_errors_fp(stderr);
    println(std::cerr, "Error loading the server private key file, possible "
                       "key/cert mismatch???");
    return EXIT_FAILURE;
  }

  SSL_CTX_set_verify(ctx.get(), SSL_VERIFY_NONE, nullptr);

  assert(!hostport.empty());

  auto acceptor_bio{
      std::unique_ptr<BIO, int (*)(BIO *)>{
          BIO_new_accept(hostport.data()),
          BIO_free,
      },
  };

  if (!acceptor_bio) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Error creating acceptor bio");
    return EXIT_FAILURE;
  }

  BIO_set_bind_mode(acceptor_bio.get(), BIO_BIND_REUSEADDR);
  if (!(BIO_do_accept(acceptor_bio.get()) > 0)) {
    ERR_print_errors_fp(stderr);
    println(std::cerr, "Error setting up acceptor socket");
    return EXIT_FAILURE;
  }

  ERR_clear_error();

  auto client_bio{
      std::unique_ptr<BIO, int (*)(BIO *)>{
          BIO_pop(acceptor_bio.get()),
          BIO_free,
      },
  };

  println(std::cout, "New client connection accepted");

  auto ssl{
      std::unique_ptr<SSL, void (*)(SSL *)>{SSL_new(ctx.get()), SSL_free},
  };

  if (!ssl) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Error creating SSL handle for new connection");
    return EXIT_FAILURE;
  }

  SSL_set_bio(ssl.get(), client_bio.get(), client_bio.get());

  if (!(SSL_accept(ssl.get()) > 0)) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Error performing SSL handshake with client");
    return EXIT_FAILURE;
  }

  size_t total{0};

  std::array<char, 1024> buf;
  for (size_t nread{0}; SSL_read_ex(ssl.get(), buf.data(), buf.size(), &nread);
       nread = 0) {
    size_t nwritten{0};
    if (SSL_write_ex(ssl.get(), buf.data(), nread, &nwritten) &&
        nwritten == nread) {
      total += nwritten;
      continue;
    }
    std::println(std::cerr, "Error echoing client input");
    break;
  }

  std::println(std::cerr, "Client connection closed, {} bytes sent", total);

  return EXIT_SUCCESS;
}
