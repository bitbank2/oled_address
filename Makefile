CFLAGS= -D__LINUX__ -c -Wall -O2
LIBS = -lm -lpthread -lOneBitDisplay -lgpiod

all: oled_addr

oled_addr: main.o
	g++ main.o $(LIBS) -o oled_addr

main.o: main.cpp
	g++ $(CFLAGS) main.cpp

clean:
	rm *.o oled_addr
