CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
MAIN = fw
OBJS = fw.o
all : $(MAIN)

$(MAIN) : $(OBJS) 
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

mytr.o : fw.c
	$(CC) $(CFLAGS) -c mytr.c

clean :
	rm *.0 $(MAIN) core*
