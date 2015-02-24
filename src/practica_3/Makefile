LIBS_DIR=libs
EXE_DIR=exe
OBJECT_DIR=obj

all: clean dirs eff_serial ineff_serial parallel

eff_serial:
	@gcc -iquote $(LIBS_DIR) -c Serial_Detector_Efficient_Cache.c
	@mv Serial_Detector_Efficient_Cache.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/Serial_Detector_Efficient_Cache $(OBJECT_DIR)/Serial_Detector_Efficient_Cache.o

ineff_serial:
	@gcc -iquote $(LIBS_DIR) -c Serial_Detector_Inefficient_Cache.c
	@mv Serial_Detector_Inefficient_Cache.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/Serial_Detector_Inefficient_Cache $(OBJECT_DIR)/Serial_Detector_Inefficient_Cache.o

parallel:
	@gcc -iquote $(LIBS_DIR) -c Parallel_Detector.c -lpthread
	@mv Parallel_Detector.o $(OBJECT_DIR)
	@gcc -o $(EXE_DIR)/Parallel_Detector $(OBJECT_DIR)/Parallel_Detector.o

clean:
	@rm -f obj/*.o
	@rm -f exe/*

dirs:
	@mkdir -p $(LIBS_DIR)
	@mkdir -p $(EXE_DIR)
	@mkdir -p $(OBJECT_DIR)
