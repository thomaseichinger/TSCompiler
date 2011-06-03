OBJS = main.o Parser.o Scanner.o
CC = g++
LD = g++
CC_FLAGS = -c -Wall
LD_FLAGS = -o 
EXECUTABLE = ../TSCompiler
COCO = ../CoCo/Coco

$(EXECUTABLE) : $(OBJS)
	$(LD) $(LD_FLAGS) $(EXECUTABLE) $(OBJS)

main.o : Parser.o Scanner.o main.cpp
	$(CC) $(CC_FLAGS) main.cpp

Parser.o : Parser.cpp Parser.h Parser.frame
	$(CC) $(CC_FLAGS) Parser.cpp

Scanner.o : Scanner.cpp Scanner.h Scanner.frame
	$(CC) $(CC_FLAGS) Scanner.cpp
	
Scanner.frame Parser.frame : TSCompiler.atg
	$(COCO) TSCompiler.atg

all : $(EXECUTABLE)

clean :
	rm $(wildcard *.o) $(EXECUTABLE) $(wildcard *.old) $(wildcard *~)

tar :
	tar -cvf ../TSCompiler.tar * && gzip -S .gz -9 ../TSCompiler.tar

doc :
	doxygen ../Doxyfile

run :
	../TSCompiler ../test/test.ts #arguments

valgrind :
	valgrind ../TSCompiler #arguments