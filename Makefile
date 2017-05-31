OBJ=tinyscheme/scheme.o scheme.o 

default: rntscheme

CFLAGS += -Itinyscheme -I$(HOME)/.runt/include -Wall -ansi
LDFLAGS += -L$(HOME)/.runt/lib -lrunt -lm -ldl

%.o: %.c 
	$(CC) $(CFLAGS) -c -DSTANDALONE=0 $< -o $@

rntscheme: $(OBJ) parse.o
	$(CC) $(OBJ) parse.o -o $@ $(LDFLAGS)

librunt_scheme.a: $(OBJ)
	$(AR) rcs $@ $(OBJ)

install: librunt_scheme.a rntscheme
	mkdir -p ~/.runt/lib
	mkdir -p ~/.runt/bin
	mkdir -p ~/.runt/include
	cp librunt_scheme.a ~/.runt/lib
	cp rntscheme ~/.runt/bin/
	cp rscheme.h ~/.runt/include/

clean:
	rm -rf $(OBJ)
	rm -rf parse.o
	rm -rf rntscheme
	rm -rf librunt_scheme.a
