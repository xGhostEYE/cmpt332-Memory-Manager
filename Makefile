# CMPT 332
# Assignment 4
# Group 38
# Tanner Laroque, YJE740, 11122821
# Melvin Lugo, 11279686, mel196

CC = gcc
CFLAGS = -c -g
CPPFLAGS = -std=gnu90 -Wall -pedantic -Wextra
OBJECT_DIRECTORY = build/obj

# Define targets
TARGETS = partA

all: $(TARGETS)

partA: A1.o monitor.o reader-writer-monitor.o list.o list_adders.o \
	list_movers.o list_removers.o
	$(CC) $(OBJECT_DIRECTORY)/A1.o $(OBJECT_DIRECTORY)/monitor.o \
	$(OBJECT_DIRECTORY)/reader-writer-monitor.o $(OBJECT_DIRECTORY)/list.o \
	$(OBJECT_DIRECTORY)/list_adders.o $(OBJECT_DIRECTORY)/list_movers.o \
	$(OBJECT_DIRECTORY)/list_removers.o -o partA \
	-L /student/cmpt332/rtt/lib/Linuxx86_64 -lRtt -lRttUtils -ltirpc -lm

A1.o: A1.c outputFolders
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -I /student/cmpt332/rtt/include \
	-I /usr/include/tirpc A1.c -o $(OBJECT_DIRECTORY)/A1.o

monitor.o: monitor.c outputFolders
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -I /student/cmpt332/rtt/include \
	-I /usr/include/tirpc monitor.c -o $(OBJECT_DIRECTORY)/monitor.o

reader-writer-monitor.o: reader-writer-monitor.c outputFolders
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . -I /student/cmpt332/rtt/include \
	-I /usr/include/tirpc reader-writer-monitor.c -o \
	$(OBJECT_DIRECTORY)/reader-writer-monitor.o

list.o: list.c outputFolders
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . list.c -o $(OBJECT_DIRECTORY)/list.o

list_adders.o: list_adders.c outputFolders
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . list_adders.c -o \
		$(OBJECT_DIRECTORY)/list_adders.o

list_movers.o: list_movers.c outputFolders
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . list_movers.c -o \
		$(OBJECT_DIRECTORY)/list_movers.o

list_removers.o: list_removers.c outputFolders
	$(CC) $(CFLAGS) $(CPPFLAGS) -I . list_removers.c -o \
	   	$(OBJECT_DIRECTORY)/list_removers.o

.PHONY: clean
clean:
	rm -rf build partA

outputFolders:
	mkdir -p $(OBJECT_DIRECTORY)

