#!/bin/sh

git clone https://github.com/bblanchon/ArduinoJson.git
cd ArduinoJson
mkdir build
cd build
cmake ..
make
sudo make install
