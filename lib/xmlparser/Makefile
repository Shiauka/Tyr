#================================
# PATH SETTING
#================================
ROOT?=../..
LIB_PATH?=$(ROOT)/lib
BIN_PATH?=$(ROOT)/bin
PROJECT=xmlparser
PROJECT_PATH=$(LIB_PATH)/$(PROJECT)


#================================
# INCLUDE
#================================
-include $(LIB_PATH)/common.mk


#================================
# PROJECT SETTING
#================================
SRC_PATH=$(PROJECT_PATH)/src\
         $(PROJECT_PATH)/expat
INC_PATH+=$(PROJECT_PATH)/src\
          $(PROJECT_PATH)/expat

OUT_BIN=$(BIN_PATH)/$(PROJECT)
VPATH=$(SRC_PATH)

OBJ_PATH=$(PROJECT_PATH)/obj
SRC_C=$(foreach dir, $(SRC_PATH), $(wildcard $(dir)/*.c))
OBJ_C_0=$(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJ_C=$(foreach file, $(OBJ_C_0), $(OBJ_PATH)/$(file))

#================================
# COMPILE SETTING
#================================
CC=gcc
CFLAGS=-g -O3 -Wall -Werror
INCLUDE:=$(foreach dir, $(INC_PATH), -I $(dir))
CC_OPTS=$(INCLUDE) $(CFLAGS)


#================================
# RULE
#================================
.PHONY: xmlparser setup clean

xmlparser: setup $(OUT_BIN)

$(OUT_BIN): $(OBJ_C)
	@$(CC) -o $@ $^
	@echo "Make $(PROJECT) project done"

$(OBJ_C): $(OBJ_PATH)/%.o:%.c
	@echo "[CC] $@"
	@$(CC) $(CC_OPTS) -c -o $@ $<;
	
setup:
	-@mkdir -p $(OBJ_PATH);
	@echo "Make $(PROJECT) project";

clean:
	-@rm -rf $(OBJ_PATH)

