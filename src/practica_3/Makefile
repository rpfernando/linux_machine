LIBS_DIR=libs
EXE_DIR=exe
OBJECT_DIR=obj

all: clean dirs eff_serial ineff_serial parallel clone

eff_serial:
	@gcc -c Serial_Detector_Efficient_Cache.c
	@mv Serial_Detector_Efficient_Cache.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/Serial_Detector_Efficient_Cache $(OBJECT_DIR)/Serial_Detector_Efficient_Cache.o -lpthread
	./$(EXE_DIR)/Serial_Detector_Efficient_Cache

ineff_serial:
	@gcc -c Serial_Detector_Inefficient_Cache.c
	@mv Serial_Detector_Inefficient_Cache.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/Serial_Detector_Inefficient_Cache $(OBJECT_DIR)/Serial_Detector_Inefficient_Cache.o -lpthread
	./$(EXE_DIR)/Serial_Detector_Inefficient_Cache

parallel:
	@gcc -c Parallel_Detector.c
	@mv Parallel_Detector.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/Parallel_Detector $(OBJECT_DIR)/Parallel_Detector.o -lpthread
	./$(EXE_DIR)/Parallel_Detector

clone:
	@gcc -c Parallel_Detector_Clone.c
	@mv Parallel_Detector_Clone.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/Parallel_Detector_Clone $(OBJECT_DIR)/Parallel_Detector_Clone.o -lpthread
	./$(EXE_DIR)/Parallel_Detector_Clone

clean:
	@rm -f obj/*.o
	@rm -f exe/*

dirs:
	@mkdir -p $(LIBS_DIR)
	@mkdir -p $(EXE_DIR)
	@mkdir -p $(OBJECT_DIR)
