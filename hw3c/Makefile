# CSF Assignment 3 Makefile

CXX = g++
CXXFLAGS = -std=c++11 -pedantic -Wall -Wextra

csim: csim.o Cache.o Set.o Sim.o
	$(CXX) -o csim csim.o Cache.o Set.o Sim.o

csim.o: csim.cpp Cache.h Sim.h 
	$(CXX) $(CXXFLAGS) -c csim.cpp

Sim.o: Sim.cpp Cache.h
	$(CXX) $(CXXFLAGS) -c Sim.cpp

Cache.o: Cache.cpp Set.h
	$(CXX) $(CXXFLAGS) -c Cache.cpp

Set.o: Set.cpp
	$(CXX) $(CXXFLAGS) -c Set.cpp

clean:
	rm -f *.o csim
