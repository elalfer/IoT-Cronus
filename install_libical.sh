#!/bin/sh

git clone https://github.com/libical/libical.git
cd libical
mkdir build
cd build
cmake ..
make
sudo make install
