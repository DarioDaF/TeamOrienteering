OBJ_FILES=src/TOP_Data.o
CPPFLAGS=-std=c++17 -O3
LDFLAGS=

Main.exe: src/Main.o $(OBJ_FILES)
	g++ -o $@ $(LDFLAGS) $^

src/TOP_Data.o: src/TOP_Data.hpp
src/Main.o: src/TOP_Data.hpp src/Utils.hpp

clean:
	rm -f Main.exe src/Main.o $(OBJ_FILES)
