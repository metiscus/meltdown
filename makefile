CXXFLAGS=-g -MMD -O0 -Wall

default: meltdown

SRC=\
	main.cpp\
	reactor.cpp\
	core.cpp\
	steamgenerator.cpp\
	pump.cpp\

OBJ=$(SRC:.cpp=.o)

meltdown: $(SRC) $(OBJ)
	g++ $(CXXFLAGS) -o meltdown $(OBJ)

clean:
	- rm -f meltdown
	- rm -f $(OBJ)

-include *.d