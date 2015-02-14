LIBS_DIR=libs
EXE_DIR=exe
OBJECT_DIR=obj

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
	@rm obj/*.o
	@rm exe/*

all:
	@clean
	@my_system
	@sh
	@getty
	@init

dirs:
	@mkdir $(LIBS_DIR)
	@mkdir $(EXE_DIR)
	@mkdir $(OBJECT_DIR)
