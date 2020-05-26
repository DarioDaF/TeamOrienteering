OBJ_FILES = src/TOP_Data.o

Main.exe: src/Main.o $(OBJ_FILES)
	g++ -o $@ $^

src/TOP_Data.o: src/TOP_Data.hpp
src/Main.o: src/TOP_Data.hpp

clean:
	rm -f Main.exe $(OBJ_FILES)
