CFLAGS     = -g -O2 -Wall -Werror -std=c17 -Wno-unused-function -Wdiscarded-qualifiers -Wincompatible-pointer-types -Wint-conversion -fno-plt
CC         = gcc
LD         = gcc
TARGET     = parser
SRC 	   = src
OBJ    	   = obj

all: $(TARGET)

$(TARGET): $(OBJ)/builder.o $(OBJ)/ast.o $(OBJ)/main.o $(OBJ)/tokenizer.o
	mkdir -p $(OBJ)
	$(LD) -o $@ $^

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean: 
	$(RM) -r $(TARGET) $(OBJ)

run:
	./$(TARGET)

.PHONY: clean all run

