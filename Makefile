all: list parser A2main light-clean

LinkedListAPI.o: src/LinkedListAPI.c
	gcc -Iinclude -c src/LinkedListAPI.c -o LinkedListAPI.o -Wall -std=c11

CalendarParser.o: src/CalendarParser.c
	gcc -Iinclude -c src/CalendarParser.c -o CalendarParser.o -Wall -std=c11

list: LinkedListAPI.o
	ar cr bin/libllist.a LinkedListAPI.o

parser: CalendarParser.o LinkedListAPI.o
	ar cr bin/libcparse.a CalendarParser.o LinkedListAPI.o
  
A2main:
	gcc -Iinclude src/A2main.c -o bin/A2main bin/libllist.a bin/libcparse.a -g -Wall -std=c11

light-clean:
	rm -f *.o *.so

clean:
	rm -f *.o *.so bin/*.a bin/A2main
