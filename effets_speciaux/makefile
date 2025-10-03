CFILES = $(wildcard *.c)
HFILES = $(wildcard *.h)
OFILES = $(patsubst %.c,%.o,$(CFILES))
GCC_FLAGS = -Wall -g 
LD_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lm

all: main

%.o: %.c $(HFILES)
	gcc $(GCC_FLAGS) -c $<

main: $(OFILES) $(HFILES)
	gcc  -o main $(OFILES) $(LD_FLAGS)

clean:
	-rm $(OFILES)
