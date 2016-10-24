CXXFLAGS=-g -MMD -O0 -Wall
LDFLAGS=

sdl2_cflags  := $(shell pkg-config --cflags sdl2)
sdl2_libs    := $(shell pkg-config --libs sdl2)

boost_libs   := -lboost_system -lboost_filesystem

CXXFLAGS := $(CXXFLAGS) $(sdl2_cflags)
LDFLAGS  := $(LDFLAGS) $(sdl2_libs) -lGL $(boost_libs)

default: meltdown

SRC=\
	main.cpp\
	reactor.cpp\
	core.cpp\
	steamgenerator.cpp\
	pump.cpp\
	tank.cpp\
	renderer_gl.cpp\
	stb/stb_image.cpp\
	stb/stb_truetype.cpp\
	vmath.cpp\
	inputsystem.cpp\

OBJ=$(SRC:.cpp=.o)

meltdown: $(SRC) $(OBJ)
	g++ $(CXXFLAGS) -o meltdown $(OBJ) $(LDFLAGS)

clean:
	- rm -f meltdown
	- rm -f $(OBJ)

-include *.d