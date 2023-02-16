CC = gcc
LD = ld
RM = rm -rf

DIR    = $(shell pwd)
FILES  = $(shell find $(DIR) -name "*.c")
OBJS   = $(patsubst %.c, %.o, $(FILES))

CFLAGS := -Wall -g
TARGET := mmap_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ -g

.PHONY:clean
clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)
