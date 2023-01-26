# all: cisj
all: vcube

vcube:  vcube.o cisj.o cisj.h smpl.o smpl.h rand.o stack.o stack.h
	gcc vcube.o smpl.o cisj.o rand.o stack.o -lm -o vcube

smpl.o: smpl.c smpl.h
	$(COMPILE.c)  -g smpl.c

cisj.o: cisj.c cisj.h
	$(COMPILE.c) -g  cisj.c

rand.o: rand.c
	$(COMPILE.c) -g rand.c

stack.o: stack.c stack.h
	$(COMPILE.c) -g stack.c

vcube.o: vcube.c cisj.h smpl.h stack.h
	$(COMPILE.c) -g vcube.c

clean:
	$(RM) *.o vcube cisj simpl rand relat saida stack