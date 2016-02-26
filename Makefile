all: geekclock 

geekclock: geekclock.c
	gcc geekclock.c -lX11 -o geekclock
#	gcc geekclock.c -c -I/usr/local/include -I/usr/X11R6/include 
#
#geekclock: geekclock.o
#	gcc -lX11 geekclock.o -o geekclock
clean: 
	rm geekclock.o geekclock


