WOLFSSL_LIB=/home/ialbrekh/Projects/polivalka/libs/wolfssl-3.7.0-bin
ICAL_LIB=../../libs/libical-bin

INCLUDE=-I$(WOLFSSL_LIB)/include -I$(ICAL_LIB)/include

CC=g++
CFLAGS=-O3 -g -std=c++11 
CLIBS=-L$(WOLFSSL_LIB)/lib -L$(ICAL_LIB)/lib -lwolfssl -lical -lical_cxx

OBJS=cal.o gpio_stub.o main.o net.o valve.o

.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

all: $(OBJS)
	g++ $(OBJS) -o polivalka $(CLIBS) 

clean:
	rm *.o
