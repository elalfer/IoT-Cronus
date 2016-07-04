WOLFSSL_LIB=/opt/lib
ICAL_LIB=/opt/lib
BIN_NAME=polivalka

INCLUDE=-I$(WOLFSSL_LIB)/include -I$(ICAL_LIB)/include -I/opt/include

CC=g++
CFLAGS=-O3 -g -std=c++11 
# CFLAGS=-O2 -g
CLIBS=-L$(WOLFSSL_LIB) -L$(ICAL_LIB) -lwolfssl -lical -lical_cxx

OBJS=cal.o main.o net.o valve.o

# Select gpio support
ifeq ($(wildcard /usr/lib/libmraa.so),)
	ALL_OBJS=$(OBJS) gpio_stub.o
else
	ALL_OBJS=$(OBJS) gpio_edison.o
	GPIO_LIBS=-lmraa
endif

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

polivalka: $(ALL_OBJS)
	g++ $(ALL_OBJS) -o $(BIN_NAME) $(CLIBS) $(GPIO_LIBS)

all: polivalka
	mkdir -p ../bin
	rm -f ../bin/$(BIN_NAME)
	ln -s ../src/$(BIN_NAME) ../bin/

mac: all
	install_name_tool -change libical.2.dylib     ../../libs/libical-bin/lib/libical.2.dylib ./$(BIN_NAME)
	install_name_tool -change libical.2.dylib     ../../libs/libical-bin/lib/libical.2.dylib ../../libs/libical-bin/lib/libical_cxx.2.dylib
	install_name_tool -change /Users/ialbrekh/Documents/Polivalka/libcal/libical-bin/lib/libical.2.dylib  ../../libs/libical-bin/lib/libical.2.dylib ../../libs/libical-bin/lib/libical_cxx.2.dylib
	install_name_tool -change libical_cxx.2.dylib ../../libs/libical-bin/lib/libical_cxx.2.dylib ./$(BIN_NAME)

	install_name_tool -change /Users/ialbrekh/Documents/Polivalka/wolfssl-3.7.0-bin/lib/libwolfssl.1.dylib $(WOLFSSL_LIB)/lib/libwolfssl.1.dylib ./$(BIN_NAME)

clean:
	rm -f polivalka
	rm -f *.o
