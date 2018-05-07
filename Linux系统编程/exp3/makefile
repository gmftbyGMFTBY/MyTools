a.out: main.c main.h libmycp.so libmytime.so libmyjump.so libmytree.so libmyrm.so libmytouch.so libmyls.so libmyfind.so libmydu.so libmygrep.so libmynano.so
	gcc -g main.c main.h -lmytouch -lmycp -lmytime -lmyjump -lmytree -lmyrm -lmyls -lmyfind -lmydu -lmygrep -lmynano -I. -L. -o a.out
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
libmyfind.so: myfind.c
	gcc --shared -fPIC -o libmyfind.so myfind.c
libmydu.so: mydu.c
	gcc --shared -fPIC -o libmydu.so mydu.c
libmygrep.so: mygrep.c
	gcc --shared -g -fPIC -o libmygrep.so mygrep.c
libmynano.so: mynano.c
	gcc --shared -g -fPIC -o libmynano.so mynano.c -lncurses
clean:
	rm a.out libmycp.so libmytime.so libmyjump.so libmytree.so libmyrm.so libmytouch.so libmyls.so libmyfind.so libmydu.so libmygrep.so libmynano.so
