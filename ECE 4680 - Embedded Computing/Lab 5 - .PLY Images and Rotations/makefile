lab5 : lab5.o
	gcc -Wall -g lab5.o -o lab5 -lm -O

lab5.o : lab5.c
	clear && make clean
	gcc -Wall -g -c lab5.c

clean :
	rm -f *.o lab5 core

run :
	./lab5 -i ../images/ant.ply -o footbones_out.ppm -c 20 30 40


