TARGET = subjust
PREFIX_BIN = /usr/local/bin

CC = gcc
INCLUDES =
LIBS =
CFLAGS =-Wall -Werror
LINKFLAGS = 

C_SOURCES = $(wildcard *.c)
C_OBJS = $(patsubst %.c, %.o, $(C_SOURCES))

.c.o:
	$(CC) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.c

compile: $(C_OBJS)
	$(CC)     $(LINKFLAGS) -o $(TARGET) $^ $(LIBS)

clean:
	rm -f $(C_OBJS)
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) $(PREFIX_BIN)

uninstall:
	rm -f $(PREFIX_BIN)/$(TARGET)

rebuild: clean compile 