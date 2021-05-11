CC = gcc
CFLAGS = -Wall -O
SRCS = $(shell find . -name  "*.c")
OBJS = $(SRCS:.c=.o)

TARGET = main

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJS) *~
