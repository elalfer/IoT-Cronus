#!/bin/sh

INSTALL_DIR_INCLUDE=/opt/include
INSTALL_DIR_LIB=/opt/lib

wget https://github.com/bblanchon/ArduinoJson/archive/master.zip
unzip master.zip

mkdir -p $INSTALL_DIR_INCLUDE/ArduinoJson
cp -R ./ArduinoJson-master/include $INSTALL_DIR_INCLUDE/ArduinoJson

rm master.zip
rm -R ArduinoJson-master
