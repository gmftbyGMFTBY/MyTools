a.out: main.c libmcp.a libmtime.a
	gcc main.c -lmcp -lmtime -I. -L. -o a.out
libmcp.a: mycp.o
	ar crs libmcp.a mycp.o
libmtime.a: mytime.o
	ar crs libmtime.a mytime.o
mycp.o: mycp.c
	gcc -c mycp.c -o mycp.o
mytime.o: mytime.c
	gcc -c mytime.c -o mytime.o
clean:
	rm a.out libmcp.a mycp.o libmtime.a mytime.o
