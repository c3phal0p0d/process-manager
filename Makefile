EXE=allocate

$(EXE): main.c process_manager.o utils.o
	cc -Wall -o $(EXE) $<

process_manager.o: process_manager.c process_manager.h
	cc -Wall -c process_manager.c -g

utils.o: utils.c utils.h process_manager.h
	cc -Wall -c utils.c -g

format:
	clang-format -style=file -i *.c
