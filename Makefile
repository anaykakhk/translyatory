all:
	gcc -g main.c
	./a.out
	#./a.out < test

clean:
	rm a.out
