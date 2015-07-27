ROOT:=$(CURDIR)
LIB_PATH=$(ROOT)/lib
BIN_PATH=$(ROOT)/bin

#================================
# SETTING LIB PATH
#================================
LIB_NAME_0=xmlparser html2xml cvs2sk skanalyser
LIB_NAME_MAKE=$(foreach file, $(LIB_NAME_0), $(file)_MAKE)
LIB_NAME_CLEAN=$(foreach file, $(LIB_NAME_0), $(file)_CLEAN)

#================================
# RULE
#================================
.PHONY: clean gen install 

all: install
	
setup:
	@mkdir -p $(BIN_PATH)

$(LIB_NAME_MAKE): %_MAKE:$(LIB_PATH)/%
	-@cd $^ && make;

$(LIB_NAME_CLEAN): %_CLEAN:$(LIB_PATH)/%
	-@cd $^ && make clean;

install: setup $(LIB_NAME_MAKE)
	@echo "install done";

clean: $(LIB_NAME_CLEAN)
	@rm -rf $(BIN_PATH) 
	@echo "clean done"
