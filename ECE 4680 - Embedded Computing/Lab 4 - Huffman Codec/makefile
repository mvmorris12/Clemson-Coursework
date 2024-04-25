lab4 : lab4.o
	gcc -Wall -g lab4.o -o lab4

lab4.o : lab4.c
	clear && make clean
	gcc -Wall -g -c lab4.c

clean :
	rm -f *.o lab4 core

enc :
# 	./lab4 -i test.txt -o test.txt.enc -c
	./lab4 -i new.out -o new.out.enc -c

dec :
#	./lab4 -i test.txt.enc -o test2.txt -d
	./lab4 -i new.out.enc -o new2.out -d
