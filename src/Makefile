WOLFSSL_LIB=/home/ialbrekh/Projects/polivalka/libs/wolfssl-3.7.0-bin
ICAL_LIB=../../libs/libical-bin

INCLUDE=-I$(WOLFSSL_LIB)/include -I$(ICAL_LIB)/include

CC=g++
CFLAGS=-O3 -g -std=c++11 
CLIBS=-L$(WOLFSSL_LIB)/lib -L$(ICAL_LIB)/lib -lwolfssl -lical -lical_cxx

OBJS=cal.o main.o net.o valve.o

# Select gpio support
ifeq ($(wildcard /bla),)
	ALL_OBJS=$(OBJS) gpio_stub.o
else
	ALL_OBJS=$(OBJS) gpio_edison.o
endif

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

polivalka: $(ALL_OBJS)
	g++ $(ALL_OBJS) -o polivalka $(CLIBS) 

all: polivalka
	mkdir -p ../bin
	rm -f ../bin/polivalka
	ln -s ../src/polivalka ../bin/

clean:
	rm -f polivalka
	rm -f *.o
