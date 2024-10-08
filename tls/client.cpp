#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <filesystem>
#include <iostream>
#include <memory>
#include <new>
#include <print>
#include <string>
#include <string_view>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <openssl/types.h>
#include <openssl/x509_vfy.h>

using namespace std::string_view_literals;

constexpr auto kPeerHostPortDefault{"localhost:3333"sv};

namespace {

[[noreturn]] void show_help(std::string_view program) {
  auto const help{
      R"HELP(
-s, --service    Host:Port of the service to connect to

--root-ca-crt    Path to the root CA certificate to verify certificates of the
                 service connected

-h, --help       Show this help page
    )HELP"sv,
  };
  std::println(std::cout, "{}:", program);
  std::println(std::cout, "{}", help);
  exit(EXIT_SUCCESS);
}

} // namespace

int main(int argc, char const *argv[]) {
  std::string_view hostport{kPeerHostPortDefault};
  std::filesystem::path root_ca_crt{".crtkey/root_ca.crt"};

  auto const first{argv + 1};
  auto const last{argv + argc};
  for (auto arg_it{first}; last != arg_it; ++arg_it) {
    if (!(strcmp(*arg_it, "-s") && strcmp(*arg_it, "--service")) &&
        (arg_it + 1) != last) {
      hostport = *(++arg_it);
    } else if (!strcmp(*arg_it, "--root-ca-crt") && (arg_it + 1) != last) {
      root_ca_crt = *(++arg_it);
    } else if (!(strcmp(*arg_it, "-h") && strcmp(*arg_it, "--help"))) {
      show_help(argv[0]);
      /* must not be here */
      std::abort();
    }
  }

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
    std::println(std::cerr,
                 "Failed to set the minimum TLSv1.3 protocol version");
    return EXIT_FAILURE;
  }

  SSL_CTX_set_verify(ctx.get(), SSL_VERIFY_PEER, nullptr);

  if (!(SSL_CTX_load_verify_file(ctx.get(), root_ca_crt.c_str()) > 0)) {
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
      std::string{
          hostport.substr(std::min(hostname.size() + 1, hostport.size())),
      },
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

  auto const msg{"Hello, world"sv};

  std::println(std::cout, "Sending message '{}' to the peer", msg);

  size_t nwritten{0};
  SSL_write_ex(ssl.get(), msg.data(), msg.size(), &nwritten);

  assert(msg.size() == nwritten);

  std::array<char, 1024> buf;
  size_t nread{0};
  SSL_read_ex(ssl.get(), buf.data(), buf.size(), &nread);

  std::println(std::cout, "Received message '{}' from the peer",
               std::string_view{buf.data(), nread});

  return 0;
}
