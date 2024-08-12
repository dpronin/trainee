#!/usr/bin/env python3

import os
import argparse
import pathlib

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='crtkeygen',
        description='Generates certificates and keys',
        epilog='support: dannftk@yandex.ru'
    )
    parser.add_argument('-o', '--output-directory',
                        type=pathlib.Path,
                        default=f'{os.getcwd()}/.crtkey',
                        help='Output directory for certificates and keys '
                        'to be generated (default: %(default)s)')

    args = parser.parse_args()

    os.system(f'mkdir -p {args.output_directory}')
    os.system("openssl version")
    os.system(f'openssl genrsa'
              f' -out {args.output_directory}/root_ca.key 2048')
    os.system(f'openssl req -x509 -new'
              f' -key {args.output_directory}/root_ca.key'
              ' -subj '
              '\"'
              '/C=RU'
              '/ST=St. Petersburg'
              '/L=St. Petersburg'
              '/O=TLS Server Inc.'
              '/OU=TLS Server Inc.'
              '/CN=localhost'
              '/CN=127.0.0.1'
              '\"'
              ' -days 365'
              f' -out {args.output_directory}/root_ca.crt')
    os.system(f'openssl genrsa'
              f' -out {args.output_directory}/server.key 2048')
    os.system(f'openssl req -new'
              f' -key {args.output_directory}/server.key'
              ' -subj \"/CN=localhost/CN=127.0.0.1\"'
              f' -out {args.output_directory}/server.csr')
    os.system(f'openssl x509 -req'
              f' -in {args.output_directory}/server.csr'
              f' -CA {args.output_directory}/root_ca.crt'
              f' -CAkey {args.output_directory}/root_ca.key'
              ' -CAcreateserial'
              f' -out {args.output_directory}/server.crt'
              ' -days 365'
              ' -extensions SAN'
              ' -extfile server.cnf')
