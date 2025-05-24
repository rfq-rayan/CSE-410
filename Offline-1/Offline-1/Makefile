CC = g++
CFLAGS = -Wall -g
LIBS = -lGL -lGLU -lglut -lm

all: clock

clock: clock.cpp
	$(CC) $(CFLAGS) -o clock clock.cpp $(LIBS)

clean:
	rm -f clock
