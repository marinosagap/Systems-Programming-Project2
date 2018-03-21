OBJS1	= jms_coord.o  functions.o
OBJS2   = jms_console.o functions.o
OUT1	= jms_coord
OUT2 	= jms_console
CC		= gcc
FLAGS	= -c -g -Wall

all: t1 t2

t1: $(OBJS1) 
	$(CC) $(OBJS1) -o $(OUT1) 
jms_coord.o: jms_coord.c
	$(CC) $(FLAGS) jms_coord.c 
functions.o: functions.c
	$(CC) $(FLAGS) functions.c

t2: $(OBJS2)
	$(CC) $(OBJS2)  -o $(OUT2) 
jms_console.o: jms_console.c
	$(CC) $(FLAGS) jms_console.c 



# clean up
clean:
	rm -f $(OBJS1) $(OUT1) 
	rm -f $(OBJS2) $(OUT2)
	
    