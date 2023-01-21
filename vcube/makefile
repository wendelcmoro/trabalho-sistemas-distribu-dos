# all: cisj
all: vcube

vcube:  vcube.o cisj.o cisj.h smpl.o smpl.h rand.o
	gcc vcube.o smpl.o cisj.o rand.o -lm -o vcube

smpl.o: smpl.c smpl.h
	$(COMPILE.c)  -g smpl.c

cisj.o: cisj.c cisj.h
	$(COMPILE.c) -g  cisj.c

rand.o: rand.c
	$(COMPILE.c) -g rand.c

vcube.o: vcube.c cisj.h smpl.h
	$(COMPILE.c) -g vcube.c

clean:
	$(RM) *.o vcube cisj simpl rand relat saida