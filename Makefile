CFLAGS= -D__LINUX__ -c -Wall -O2
LIBS = -lm -lpthread -lOneBitDisplay -lgpiod

all: oled_addr

oled_addr: main.o
	g++ main.o $(LIBS) -o oled_addr ;\
	sudo chmod 4755 oled_addr ;\
	sudo cp oled_addr /usr/local/bin

main.o: main.cpp
	g++ $(CFLAGS) main.cpp

clean:
	rm *.o oled_addr
