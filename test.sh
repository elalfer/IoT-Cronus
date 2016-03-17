#!/bin/sh

WOLFSSL_HOME=/Users/ialbrekh/Documents/Polivalka/wolfssl-3.7.0-bin
LIB_ICAL=/Users/ialbrekh/Documents/Polivalka/libcal/libical-bin/lib
INCLUDE_ICAL=/Users/ialbrekh/Documents/Polivalka/libcal/libical-bin/include

g++ -O2 -c ./cal.cpp -I$WOLFSSL_HOME/include -I$INCLUDE_ICAL
g++ -O2 -c ./net.cpp -I$WOLFSSL_HOME/include -I$INCLUDE_ICAL
g++ -O2 -c ./valve.cpp -I$WOLFSSL_HOME/include -I$INCLUDE_ICAL
g++ -O2 ./main.cpp ./cal.o ./net.o ./valve.o -L$WOLFSSL_HOME/lib -I$WOLFSSL_HOME/include -lwolfssl -L$LIB_ICAL -I$INCLUDE_ICAL -lical -lical_cxx

install_name_tool -change libical.2.dylib /Users/ialbrekh/Documents/Polivalka/libcal/libical-bin/lib/libical.2.dylib ./a.out
install_name_tool -change libical.2.dylib /Users/ialbrekh/Documents/Polivalka/libcal/libical-bin/lib/libical.2.dylib /Users/ialbrekh/Documents/Polivalka/libcal/libical-bin/lib/libical_cxx.2.dylib
install_name_tool -change libical_cxx.2.dylib /Users/ialbrekh/Documents/Polivalka/libcal/libical-bin/lib/libical_cxx.2.dylib ./a.out
install_name_tool -change /Users/ialbrekh/Documents/wolfssl-3.7.0-bin/lib/libwolfssl.1.dylib $WOLFSSL_HOME/lib/libwolfssl.1.dylib ./a.out

# ./a.out
