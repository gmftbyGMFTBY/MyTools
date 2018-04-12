a.out: main.c libmcp.a
	gcc main.c -lmcp -I. -L. -o a.out
libmcp.a: cp.o
	ar crs libmcp.a cp.o
cp.o: cp.c
	gcc -c cp.c
clean:
	rm a.out libmcp.a cp.o
