# compiler variables
CCFLAGS = -Wall -std=c++11

# rule to link object code files to create exectuable file
null.exe: null.o
	g++ $(CCFLAGS) -o null.exe null.o

# rules to compile source code files to object code
null.o: null.cpp null.h 
	g++ $(CCFLAGS) -c null.cpp
	
# pseudo-target to remove object code and exectuable files
clean:
	-rm *.o null.exe