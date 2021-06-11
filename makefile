TOOLCHAIN_PREIFX = arm-none-eabi-
CC = $(TOOLCHAIN_PREIFX)gcc
CXX = $(TOOLCHAIN_PREIFX)g++
LD = $(TOOLCHAIN_PREIFX)ld
SIZE = $(TOOLCHAIN_PREIFX)size

# Faust DSP class
DSP_CLASS_NAME = panel

# Output elf file
ELF = synth.elf

# CPU type related flags
CPU_FLAGS = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -mfloat-abi=hard -mlittle-endian

# Linker script
LDSCRIPT = ./cubemx/STM32F405RGTx_FLASH.ld

# Include directories
INC = \
	-I. \
	-Icubemx/Inc \
	-Icubemx/Drivers/CMSIS/Include \
	-Icubemx/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	-Icubemx/Drivers/STM32F4xx_HAL_Driver/Inc \
	-Icubemx/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy

# Libraries
LIBS = -lc -lm -lnosys 

# Actual project sources
SRC = \
	synth.cpp \
	audio.cpp \
	analog.cpp \
	aic23b.c \
	midi.cpp

# Driver sources + Cube code
SYS_SRC = \
	cubemx/startup_stm32f405xx.s \
	$(wildcard cubemx/Drivers/STM32F4xx_HAL_Driver/Src/*.c ) \
	$(wildcard cubemx/Src/*.c )
	
# Defines
DEFS = \
	-DUSE_HAL_DRIVER \
	-DSTM32F405xx \
	-DDSP_CLASS_NAME=$(DSP_CLASS_NAME)
	
# C++ compiler flags used when compiling 
CXXFLAGS = $(CPU_FLAGS) $(DEFS) $(INC) $(LIBS) \
	-Wall \
	-O3 \
	--std=c++17 \
	-ffast-math \
	-fno-math-errno \
	--exceptions

# Compiler flags used when linking
LDFLAGS = -specs=nosys.specs -T$(LDSCRIPT) $(CPU_FLAGS) $(LIBS) -Wl,--gc-sections -flto -Wl,-u_printf_float

# Temporary object files directory
OBJDIR = obj

# Faust compiler
FAUST_CLASS_PREFIX = faust_dsp_
FAUST_BASE_CLASS   = faust_dsp_base
FAUST_MATH_HEADER  = ../fast_math.hpp

# ======
	
# Required object files
OBJECTS := $(patsubst %.cpp, $(OBJDIR)/%.o, $(SRC))
OBJECTS := $(patsubst %.c, $(OBJDIR)/%.o, $(OBJECTS))
OBJECTS := $(patsubst %.s, $(OBJDIR)/%.o, $(OBJECTS))
SYS_OBJECTS := $(patsubst %.cpp, $(OBJDIR)/%.o, $(SYS_SRC))
SYS_OBJECTS := $(patsubst %.c, $(OBJDIR)/%.o, $(SYS_OBJECTS))
SYS_OBJECTS := $(patsubst %.s, $(OBJDIR)/%.o, $(SYS_OBJECTS))

FAUST_FILES = $(wildcard faust/*.faust) $(wildcard faust/*.dsp)
FAUST_HEADERS := $(patsubst %.faust, %.hpp, $(FAUST_FILES))
FAUST_HEADERS := $(patsubst %.dsp, %.hpp, $(FAUST_HEADERS))

# Dependency control
DEPS := $(SRC) $(SYS_SRC)
DEPS := $(patsubst %.cpp, deps/%.cpp.d, $(DEPS))
DEPS := $(patsubst %.c, deps/%.c.d, $(DEPS))
DEPS := $(patsubst %.s, deps/%.s.d, $(DEPS))


all: $(ELF)
	$(SIZE) $(ELF)

$(ELF): $(FAUST_HEADERS) $(OBJECTS) $(SYS_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(SYS_OBJECTS)
		
clean:
	-rm -rf deps
	-rm -rf $(OBJDIR)
	-rm -rf faust/*.hpp faust/*.h
	-rm $(ELF)

prog:
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg -c "program $(ELF) verify reset exit"

faust/%.hpp: faust/%.dsp
	faust -cn $(patsubst %.dsp,$(FAUST_CLASS_PREFIX)%,$(notdir $<)) -scn $(FAUST_BASE_CLASS) -fm $(FAUST_MATH_HEADER) -light $< -o faust/$(notdir $@)

# 
deps/synth.cpp.d: synth.cpp $(FAUST_HEADERS)
obj/synth.o: synth.cpp $(FAUST_HEADERS)
include $(DEPS)

deps/%.cpp.d: %.cpp
	-mkdir -p $(dir $@)
	set -e; rm -f $@; \
    $(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$

deps/%.c.d: %.c
	-mkdir -p $(dir $@)
	set -e; rm -f $@; \
    $(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$

deps/%.s.d: %.s
	-mkdir -p $(dir $@)
	set -e; rm -f $@; \
    $(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$

$(OBJDIR)/%.o: %.cpp
	-mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$(OBJDIR)/%.o: %.c
	-mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.s
	-mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: prog clean
	
