a.out: main.c main.h libmycp.so libmytime.so libmyjump.so libmytree.so libmyrm.so libmytouch.so libmyls.so
	gcc -g main.c main.h -lmytouch -lmycp -lmytime -lmyjump -lmytree -lmyrm -lmyls -I. -L. -o a.out
libmycp.so: mycp.c
	gcc --shared -fPIC -o libmycp.so mycp.c
libmytime.so: mytime.c
	gcc --shared -fPIC -o libmytime.so mytime.c
libmyjump.so: myjump.c
	gcc --shared -fPIC -o libmyjump.so myjump.c
libmytree.so: mytree.c
	gcc --shared -fPIC -o libmytree.so mytree.c
libmyrm.so: myrm.c
	gcc --shared -fPIC -o libmyrm.so myrm.c
libmytouch.so: mytouch.c
	gcc --shared -fPIC -o libmytouch.so mytouch.c
libmyls.so: myls.c
	gcc --shared -fPIC -o libmyls.so myls.c
clean:
	rm a.out libmycp.so libmytime.so libmyjump.so libmytree.so libmyrm.so libmytouch.so libmyls.so
