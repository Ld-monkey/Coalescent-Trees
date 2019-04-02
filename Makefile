CC = gcc
CFLAGS = -W -Wall -lm -I$(INC_DIR)
INC_DIR = inc
OBJ_DIR = obj
SRC_DIR = src
OUTPUT = output
SOURCE = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst %, $(OBJ_DIR)/%, $(notdir $(SOURCE:.c=.o)))
TITLE = generator_phylogenetic_tree

$(TITLE): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -rf $(OBJ_DIR)/*~
	rm -rf *~
	rm -rf $(SRC_DIR)/*~
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(OUTPUT)/*.nwk

help :
	@echo "src : $(SOURCE)"
	@echo "obj : $(OBJECTS)"
