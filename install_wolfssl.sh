#!/bin/sh

git clone https://github.com/wolfSSL/wolfssl.git
cd wolfssl
sh autogen.sh
sh configure
make
make check
sudo make install
