CC = gcc
CFLAGS = -Wall -std=gnu99 -pedantic -g
MAIN = mytar
OBJS = mytar.o
all : $(MAIN)

$(MAIN) : $(OBJS) 
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

mytr.o : mytar.c
	$(CC) $(CFLAGS) -c mytr.c

clean :
	rm *.0 $(MAIN) core*
