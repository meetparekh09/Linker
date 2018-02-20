lab1: main.c read.c pass.c errors.c
	gcc -std=c99 -o lab1 main.c read.c errors.c pass.c


clean:
	rm lab1