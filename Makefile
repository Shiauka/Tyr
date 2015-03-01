ROOT:=$(CURDIR)
LIB_PATH=$(ROOT)/lib
BIN_PATH=$(ROOT)/bin

LIB_NAME_0=xmlparser html2xml
LIB_NAME_MAKE=$(foreach file, $(LIB_NAME_0), $(file)_MAKE)
LIB_NAME_CLEAN=$(foreach file, $(LIB_NAME_0), $(file)_CLEAN)

#================================
# RULE
#================================
.PHONY: clean gen install 

all: setup gen

gen: 
	@echo "gen stock db"
	
setup:
	@echo "setup stock db env";

$(LIB_NAME_MAKE): %_MAKE:$(LIB_PATH)/%
	-@cd $^ && make;

$(LIB_NAME_CLEAN): %_CLEAN:$(LIB_PATH)/%
	-@cd $^ && make clean;

install: $(LIB_NAME_MAKE)
	@echo "install done";

clean: $(LIB_NAME_CLEAN)
	@echo "clean done"

