CC = gcc
CFLAGS = -W -Wall -lm -I$(INC_DIR)
INC_DIR = inc
SRC_DIR = src
OUTPUT = output
SOURCE = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst %, $(SRC_DIR)/%, $(notdir $(SOURCE:.c=.o)))
TITLE = generator_phylogenetic_tree

$(TITLE): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

$(INC_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -rf $(SRC_DIR)/*.o
	rm -rf $(OUTPUT)/*.nwk

help :
	@echo "src : $(SOURCE)"
	@echo "obj : $(OBJECTS)"
