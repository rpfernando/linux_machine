LIBS_DIR=libs
EXE_DIR=exe
OBJECT_DIR=obj

all: clean dirs my_system sh getty init
	@./exe/init

sh:
	@gcc -iquote $(LIBS_DIR) -c sh.c
	@mv sh.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/sh $(OBJECT_DIR)/sh.o $(OBJECT_DIR)/my_system.o

getty:
	@gcc -iquote $(LIBS_DIR) -c getty.c
	@mv getty.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/getty $(OBJECT_DIR)/getty.o $(OBJECT_DIR)/my_system.o

init:
	@gcc -iquote $(LIBS_DIR) -c init.c
	@mv init.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/init $(OBJECT_DIR)/init.o $(OBJECT_DIR)/my_system.o

my_system:
	@gcc -c $(LIBS_DIR)/my_system.c
	@mv my_system.o $(OBJECT_DIR)

clean:
	@rm -f obj/*.o
	@rm -f exe/*

dirs:
	@mkdir -p $(LIBS_DIR)
	@mkdir -p $(EXE_DIR)
	@mkdir -p $(OBJECT_DIR)
