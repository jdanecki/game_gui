ALCHEMIST=../alchemist/elements.cpp ../alchemist/el_list.cpp ../alchemist/names.cpp ../alchemist/game_time.cpp
TOOLS=../../server/cpp-src/tools/axe.cpp ../../server/cpp-src/tools/axe_blade.cpp ../../server/cpp-src/tools/axe_handle.cpp \
	  ../../server/cpp-src/tools/knife.cpp ../../server/cpp-src/tools/knife_blade.cpp ../../server/cpp-src/tools/knife_handle.cpp
CORE=../world.cpp ../tiles.cpp
SERVER=../../server/cpp-src/networking.cpp ../../server/cpp-src/elements_server.cpp

all:
	g++ -Wall -g -o alchemist \
	-I../alchemist $(ALCHEMIST) \
	-I../../server/cpp-src/tools $(TOOLS) \
	-I../../server/cpp-src $(SERVER) \
	-I.. $(CORE) \
	-I. main.cpp test_axe.cpp 

clean:
	rm alchemist

