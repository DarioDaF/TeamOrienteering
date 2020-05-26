OBJ_FILES = src/TOP_Data.o

Main.exe: src/Main.o $(OBJ_FILES)
	g++ -o $@ $^

clean:
	rm -f Main.exe $(OBJ_FILES)
