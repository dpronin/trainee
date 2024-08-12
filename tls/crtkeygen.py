#!/usr/bin/env python3

from os import *

if __name__ == "__main__":
    system("mkdir -p .crtkey")
    system("openssl version")
    system("openssl genrsa"
           " -out .crtkey/root_ca.key 2048")
    system("openssl req -x509 -new"
           " -key .crtkey/root_ca.key"
           " -days 365"
           " -out .crtkey/root_ca.crt")
    system("openssl genrsa"
           " -out .crtkey/server.key 2048")
    system("openssl req -new"
           " -key .crtkey/server.key"
           " -subj \"/CN=localhost/CN=127.0.0.1\""
           " -out .crtkey/server.csr")
    system("openssl x509 -req"
           " -in .crtkey/server.csr"
           " -CA .crtkey/root_ca.crt"
           " -CAkey .crtkey/root_ca.key"
           " -CAcreateserial"
           " -out .crtkey/server.crt"
           " -days 365"
           " -extensions SAN"
           " -extfile server.cnf")
