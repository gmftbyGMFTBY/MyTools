a.out: main.c libmcp.a libmtime.a
	gcc main.c -lmcp -lmtime -I. -L. -o a.out
libmcp.a: cp.o
	ar crs libmcp.a cp.o
libmtime.a: mytime.o
	ar crs libmtime.a mytime.o
cp.o: cp.c
	gcc -c cp.c -o cp.o
mytime.o: mytime.c
	gcc -c mytime.c -o mytime.o
clean:
	rm a.out libmcp.a cp.o libmtime.a mytime.o
