CC = gcc
CFLAGS = -Wall -O3
SRCS = $(shell find . -name  "*.c")
OBJS = $(SRCS:.c=.o)

TARGET = count

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJS) *~