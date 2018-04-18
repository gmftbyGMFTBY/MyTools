a.out: main.c libmcp.a libmtime.a libmyjump.a libmytree.a
	gcc -g main.c -lmcp -lmtime -lmyjump -lmytree -I. -L. -o a.out
libmcp.a: mycp.o
	ar crs libmcp.a mycp.o
libmtime.a: mytime.o
	ar crs libmtime.a mytime.o
libmyjump.a: myjump.o
	ar crs libmyjump.a myjump.o
libmytree.a: mytree.o
	ar crs libmytree.a mytree.o
mycp.o: mycp.c
	gcc -c mycp.c -o mycp.o
mytime.o: mytime.c
	gcc -c mytime.c -o mytime.o
myjump.o: myjump.c
	gcc -c myjump.c -o myjump.o
mytree.o: mytree.c
	gcc -c mytree.c -o mytree.o
clean:
	rm a.out libmcp.a mycp.o libmtime.a mytime.o libmyjump.a myjump.o mytree.o libmytree.a
