TARGET_FILE := srv_test

BIN_DIR := ./bin
OBJ_DIR := ./obj
CP_DIR  := /home/q/temp

SRC_DIR := ./ \
	    ../classes \
	    ./dspemul

INCLUDE_DIR := ./ \
	    ../classes \
	    ./dsplink \
	    ./dsplink/usr \
	    ./dsplink/Linux \
	    ./dspemul \
	    ./boost \
		./alsa \
		./RCF \
		./SF

LIBS := -lpthread \
		-L./dsplink -ldsplink -lcmem \
		-L./icu-lib/ -licui18n -licuuc -licudata \
		-L./boost-lib/ -lboost_system -lboost_regex \
		-L./alsa-libs/ -lasound \
		-L./ -lrcf_arm \
		-L./libproto/ -lprotobuf
		
CFLAGS := -D__linux__ -DRCF_USE_PROTOBUF

CCLDFLAGS := -lrt -ldl 

CPP_FILES_WITH_DIR := $(addsuffix /*.cpp, $(SRC_DIR))
C_FILES_WITH_DIR := $(addsuffix /*.c, $(SRC_DIR))
CPP_OBJ_FILES := $(notdir $(patsubst %.cpp, %.o, $(wildcard $(CPP_FILES_WITH_DIR))))
C_OBJ_FILES   := $(notdir $(patsubst %.c, %.o, $(wildcard $(C_FILES_WITH_DIR))))

OBJ_FILES_WITH_DIR := $(addprefix $(OBJ_DIR)/, $(C_OBJ_FILES))
OBJ_FILES_WITH_DIR += $(addprefix $(OBJ_DIR)/, $(CPP_OBJ_FILES))

CC := arm-none-linux-gnueabi-g++

VPATH := $(SRC_DIR)

all: $(TARGET_FILE)

$(TARGET_FILE): $(OBJ_FILES_WITH_DIR)
	$(CC) -g $(CCLDFLAGS) $^ -o $(BIN_DIR)/$(TARGET_FILE) $(LIBS)

#	rm --force $(addprefix $(CP_DIR)/,$(TARGET_FILE))
#	cp $(addprefix $(BIN_DIR)/,$(TARGET_FILE)) $(CP_DIR)


$(OBJ_DIR)/%.o:%.c
	$(CC) -g -O3 -rdynamic -fno-omit-frame-pointer -funwind-tables -c -mapcs-frame -MMD -frtti -fexceptions -mfloat-abi=softfp -mfpu=neon $(CFLAGS) $(addprefix -I, $(INCLUDE_DIR)) $(addprefix -I, $(SRC_DIR)) $< -o $@
$(OBJ_DIR)/%.o:%.cpp
	$(CC) -g -O3 -rdynamic -fno-omit-frame-pointer -funwind-tables -c -mapcs-frame -MMD -frtti -fexceptions -Wno-write-strings -mfloat-abi=softfp -mfpu=neon $(CFLAGS) $(addprefix -I, $(INCLUDE_DIR)) $(addprefix -I, $(SRC_DIR)) $< -o $@
include $(wildcard $(OBJ_DIR)/*.d)

#$(OBJ_DIR)/%.o:%.c
#	$(CC) -ggdb3 -O0 -rdynamic -fno-omit-frame-pointer -funwind-tables -c -mapcs-frame -MMD -frtti -fexceptions -mfloat-abi=softfp -mfpu=neon $(CFLAGS) $(addprefix -I, $(INCLUDE_DIR)) $(addprefix -I, $(SRC_DIR)) $< -o $@
#$(OBJ_DIR)/%.o:%.cpp
#	$(CC) -ggdb3 -O0 -rdynamic -fno-omit-frame-pointer -funwind-tables -c -mapcs-frame -MMD -frtti -fexceptions -Wno-write-strings -mfloat-abi=softfp -mfpu=neon $(CFLAGS) $(addprefix -I, $(INCLUDE_DIR)) $(addprefix -I, $(SRC_DIR)) $< -o $@
#include $(wildcard $(OBJ_DIR)/*.d)

clean:
	rm --force $(OBJ_DIR)/*.d
	rm --force $(OBJ_DIR)/*.o
	rm --force $(BIN_DIR)/$(TARGET_FILE)
	rm --force $(BIN_DIR)/$(TARGET_FILE).gdb
