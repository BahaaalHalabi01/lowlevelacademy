TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default 
				./$(TARGET) -f ./local.db -n
				./$(TARGET) -f ./local.db -a "Timmy G.,Memory Lane lt.,120"
				./$(TARGET) -f ./local.db -a "Test G.,Test Lane lt.,420"
				./$(TARGET) -f ./local.db -r "Timmy" -l

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db


$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude
