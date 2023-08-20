CC = g++
CFLAGS = -std=c++11

TARGET = hw3

SRCS = main.cpp filesystem.hpp

OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm hw3
	rm main.o
	rm mySystem.dat
