EXE=allocate

$(EXE): main.c process_manager.o utils.o queue.o
	cc -Wall -o $(EXE) main.c process_manager.o utils.o queue.o -lm

process_manager.o: process_manager.c process_manager.h
	cc -Wall -c process_manager.c

utils.o: utils.c utils.h process_manager.h
	cc -Wall -c utils.c

queue.o: queue.c queue.h process_manager.h
	cc -Wall -c queue.c

format:
	clang-format -style=file -i *.c

clean:
	rm -f *.o allocate
