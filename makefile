trail: trail.o
	gcc -Wall -o trail trail.o
trail.o:
	gcc -Wall -c trail.c
clean:
	rm trail *.o random random.pgm random-trail.pgm data.pgm data-trail.pgm
