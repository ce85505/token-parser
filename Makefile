CC = gcc
CFLAGS  = -Wall

TARGET = frontEnd

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c scanner.c parser.c testTree.c

clean:
	$(RM) $(TARGET)