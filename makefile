OBJ = display.o rrtMaker.o rrtTester.o

all: display maker tester
	gcc -o rrtTester $(OBJ) -lX11 -lm

display: display.c obstacles.h display.h
	gcc -c display.c

maker: rrtMaker.c obstacles.h display.h
	gcc -c rrtMaker.c
	
tester: rrtTester.c obstacles.h display.h
	gcc -c rrtTester.c

clean:
	rm -f $(OBJ) rrtTester
