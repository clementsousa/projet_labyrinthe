CC=gcc
CFLAGS=-c -Wall -Wextra -g  
LDFLAGS= -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lm -fsanitize=address
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main.out


.PHONY : clean

all: clean $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
.c.o:
	$(CC) $(CFLAGS)  -o $@ -c $<

clean :
	rm -f $(OBJETS) $(EXECUTABLE) *.o
