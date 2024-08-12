#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <memory>
#include <new>
#include <openssl/bio.h>
#include <openssl/tls1.h>
#include <openssl/x509_vfy.h>
#include <print>
#include <string_view>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/types.h>

int main(int argc, char const *argv[]) {
  std::string_view hostport;

  if (argc > 1)
    hostport = argv[1];

  if (hostport.empty())
    hostport = "localhost:3333";

  auto ctx{
      std::unique_ptr<SSL_CTX, void (*)(SSL_CTX *)>{
          SSL_CTX_new(TLS_client_method()),
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
    std::println(std::cerr, "Failed to set the minimum TLS protocol version");
    return EXIT_FAILURE;
  }

  SSL_CTX_set_verify(ctx.get(), SSL_VERIFY_PEER, nullptr);

  if (!(SSL_CTX_load_verify_file(ctx.get(), ".crtkey/root_ca.crt") > 0)) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr,
                 "Failed to set the default trusted certificate store");
    return EXIT_FAILURE;
  }

  auto ssl{
      std::unique_ptr<SSL, void (*)(SSL *)>{SSL_new(ctx.get()), SSL_free},
  };

  if (!ssl) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Error creating SSL handle for new connection");
    return EXIT_FAILURE;
  }

  assert(!hostport.empty());

  auto const hostname{std::string{hostport.substr(0, hostport.find(':'))}};
  auto const port{
      hostport.substr(std::min(hostname.size() + 1, hostport.size())),
  };

  addrinfo hints{};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  auto services{
      std::unique_ptr<addrinfo, void (*)(addrinfo *)>{
          nullptr,
          freeaddrinfo,
      },
  };

  if (addrinfo * result{nullptr};
      getaddrinfo(hostname.data(), port.data(), &hints, &result) == 0) {
    services.reset(result);
  } else {
    perror("Failed getaddrinfo() call");
    return EXIT_FAILURE;
  }

  assert(static_cast<bool>(services));

  auto sock{
      std::unique_ptr<int const, void (*)(int const *pfd)>{
          nullptr,
          +[](int const *pfd) {
            close(*pfd);
            delete pfd;
          },
      },
  };

  for (auto *p_srv = services.get(); p_srv; p_srv = p_srv->ai_next) {
    int const fd{socket(AF_INET, SOCK_STREAM, 0)};
    if (fd < 0) {
      perror("couldn't open a new socket");
      return EXIT_FAILURE;
    }

    if (connect(fd, p_srv->ai_addr, p_srv->ai_addrlen) < 0) {
      close(fd);
      continue;
    }

    sock.reset(new (std::nothrow) int{fd});
    assert(static_cast<bool>(sock));

    break;
  }

  if (!static_cast<bool>(sock)) {
    std::println("couldn't connect to {}", hostport);
    return EXIT_FAILURE;
  }

  auto bio{
      std::unique_ptr<BIO, int (*)(BIO *)>{
          BIO_new(BIO_s_socket()),
          BIO_free,
      },
  };
  assert(static_cast<bool>(bio));

  BIO_set_fd(bio.get(), *sock, BIO_NOCLOSE);

  SSL_set_bio(ssl.get(), bio.get(), bio.get());
  bio.release();

  if (!SSL_set_tlsext_host_name(ssl.get(), hostname.data())) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Failed to set the SNI hostname '{}'", hostname);
    return EXIT_FAILURE;
  }

  if (!SSL_set1_host(ssl.get(), hostname.data())) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr,
                 "Failed to set the certificate verification hostname '{}'",
                 hostname);
    return EXIT_FAILURE;
  }

  if (SSL_connect(ssl.get()) < 1) {
    ERR_print_errors_fp(stderr);
    std::println(std::cerr, "Failed to connect to the server '{}'", hostname);
    if (SSL_get_verify_result(ssl.get()) != X509_V_OK) {
      std::println(
          std::cerr, "Verify error: %s\n",
          X509_verify_cert_error_string(SSL_get_verify_result(ssl.get())));
    }
    return EXIT_FAILURE;
  }

  using namespace std::string_view_literals;

  auto msg{"Hello, world"sv};

  size_t nwritten{0};
  SSL_write_ex(ssl.get(), msg.data(), msg.size(), &nwritten);

  assert(msg.size() == nwritten);

  std::array<char, 1024> buf;
  size_t nread{0};
  SSL_read_ex(ssl.get(), buf.data(), buf.size(), &nread);

  msg = {buf.data(), nread};

  std::println(std::cout, "echo from '{}': '{}'", hostname, msg);

  return 0;
}
