# supporting multiple STM32 boards
# STM32F405RGT (original) = HWORIG
# STM32F469I-DISCO (dev) = HWDISCO
# STM32F469ZIT6 (harp) = HWHARP

TOOLCHAIN_DIR = /opt/arm-gnu-toolchain-12.3.rel1-x86_64-arm-none-eabi/bin
TOOLCHAIN_PREIFX = $(TOOLCHAIN_DIR)/arm-none-eabi-
CC = $(TOOLCHAIN_PREIFX)gcc
CXX = $(TOOLCHAIN_PREIFX)g++
LD = $(TOOLCHAIN_PREIFX)ld
SIZE = $(TOOLCHAIN_PREIFX)size

# Output elf file
ELF = synth.elf

# CPU type related flags
CPU_FLAGS = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -mfloat-abi=hard -mlittle-endian

# ORIGINAL SYNTH !
ifdef HWORIG

# Faust DSP class
# will change later
DSP_CLASS_NAME = panel

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

# Actual project sources
SRC = \
	synth.cpp\
	audio.cpp \
	analog.cpp \
	aic23b.cpp \
	midi.cpp

FAUST_FILES = $(wildcard faust/*.faust) $(wildcard faust/*.dsp)

endif

# DISCOVERY BOARD BSP
ifdef HWDISCO

# Faust DSP class
# will change later
DSP_CLASS_NAME = sine

# Linker script
STMTOOLS = /opt/st/stm32cubeide_1.13.2/plugins/com.st.stm32cube.ide.mcu.externaltoools.gnu-tools-for-stm32.11.3.rel1.linux64_1.1.1.202309131626/tools
LDSCRIPT = BSP/SW4STM32/STM32469I_DISCO/STM32F469NIHx_FLASH.ld

# Include directories
INC = \
	-I. \
	-IBSP/Inc \
	-IBSP/Drivers/CMSIS/Include \
	-IBSP/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	-IBSP/Drivers/STM32F4xx_HAL_Driver/Inc \
	-IBSP/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \
	-IBSP/Utilities/Log \
	-IBSP/Utilities/Fonts \
	-IBSP/Drivers/BSP/Components/Common \
	-IBSP/Drivers/BSP/STM32469I-Discovery \
	-IBSP/Middlewares/ST/STM32_Audio/Addons/PDM/Inc \
	-I$(STMTOOLS)/arm-none-eabi/include/c++/11.3.1 \
	-I$(STMTOOLS)/arm-none-eabi/include/c++/11.3.1/arm-none-eabi/thumb/nofp \
	-I$(STMTOOLS)/arm-none-eabi/include/c++/11.3.1/backward

	
# Driver sources + Cube code
SYS_SRC = \
	BSP/SW4STM32/startup_stm32f469xx.s \
	BSP/SW4STM32/syscalls.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac_ex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma2d.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dsi.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_ltdc_ex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_qspi.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sai.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sai_ex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sd.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_sdram.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_fmc.c \
BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_sdmmc.c \
	$(wildcard BSP/Drivers/BSP/STM32469I-Discovery/*.c ) \
	$(wildcard BSP/Drivers/BSP/Components/**/*.c ) \
	$(wildcard BSP/Drivers/BSP/CMSIS/*.c ) \
	$(wildcard BSP/Utilities/Log/*.c)

# Defines
DEFS = \
	-DUSE_HAL_DRIVER \
	-DSTM32F469xx \
	-DUSE_STM32469I_DISCO \
	-DUSESTM324691_DISCO_REVC \
	-DTS_MULTI_TOUCH_SUPPORTED \
	-DDSP_CLASS_NAME=$(DSP_CLASS_NAME)
	
# Actual project sources
SRC = \
	$(wildcard harp/*.c ) \
	$(wildcard harp/*.cpp ) 

FAUST_FILES = \
	faust/sine.dsp \
	faust/ks_harp.dsp


endif

ifdef HWHARP
# Faust DSP class
# will change later
DSP_CLASS_NAME = sine

# Linker script
STMTOOLS = /opt/st/stm32cubeide_1.13.2/plugins/com.st.stm32cube.ide.mcu.externaltoools.gnu-tools-for-stm32.11.3.rel1.linux64_1.1.1.202309131626/tools
LDSCRIPT = stringless_harp/STM32F469ZITX_FLASH.ld

# Include directories
INC = \
	-I. \
	-Iharp_zit6/Inc \
	-Istringless_harp/Drivers/CMSIS/Include \
	-Istringless_harp/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	-Istringless_harp/Drivers/STM32F4xx_HAL_Driver/Inc \
	-Istringless_harp/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \
	-Istringless_harp/FATFS/App \
	-Istringless_harp/FATFS/Target \
	-Istringless_harp/Middlewares/Third_Party/FatFs/src
	
# Driver sources + Cube code
SYS_SRC = \
	stringless_harp/Core/Startup/startup_stm32f469zitx.s \
	stringless_harp/Core/Src/syscalls.c \
	$(wildcard stringless_harp/Drivers/STM32F4xx_HAL_Driver/Src/*.c) \
	$(wildcard stringless_harp/Middlewares/Third_Party/FatFs/src/*.c) \
	stringless_harp/Middlewares/Third_Party/FatFs/src/option/syscall.c

# Defines
DEFS = \
	-DUSE_HAL_DRIVER \
	-DSTM32F469xx \
	-DDSP_CLASS_NAME=$(DSP_CLASS_NAME)
	
# Actual project sources
SRC = \
	$(wildcard harp_zit6/Src/*.c) \
	$(wildcard stringless_harp/FATFS/**/*.c)
	

FAUST_FILES = \
	faust/sine.dsp \
	faust/ks_harp.dsp

endif

# Libraries
LIBS = -lc -lm -lnosys 

# C++ compiler flags used when compiling 
CXXFLAGS = $(CPU_FLAGS) $(DEFS) $(INC) $(LIBS) \
	-Wall \
	-O3 \
	--std=c++17 \
	-ffast-math \
	-fno-math-errno \
	--exceptions \
	-g
	
CCFLAGS = $(CPU_FLAGS) $(DEFS) $(INC) $(LIBS) \
	-Wall \
	-O3 \
	-ffast-math \
	-fno-math-errno \
	--exceptions \
	-g

# Compiler flags used when linking
LDFLAGS = -specs=nosys.specs -T$(LDSCRIPT) $(CPU_FLAGS) $(LIBS) -Wl,--gc-sections -flto -Wl,-u_printf_float -Wl,--no-warn-rwx-segment

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

# could change / remove based on how we prog later
prog:
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg -c "program $(ELF) verify reset exit"

faust/%.hpp: faust/%.dsp
	faust -cn $(patsubst %.dsp,$(FAUST_CLASS_PREFIX)%,$(notdir $<)) -scn $(FAUST_BASE_CLASS) -fm $(FAUST_MATH_HEADER) -light $< -o faust/$(notdir $@)

# 
deps/synth.cpp.d: synth.cpp $(FAUST_HEADERS)
obj/synth.o: synth.cpp $(FAUST_HEADERS)
-include $(DEPS)

deps/%.cpp.d: %.cpp
	-mkdir -p $(dir $@)
	set -e; rm -f $@; \
    $(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -f $@.$$$$

deps/%.c.d: %.c
	-mkdir -p $(dir $@)
	set -e; rm -f $@; \
    $(CC) -MM $(CCFLAGS) $< > $@.$$$$; \
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
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.s
	-mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: prog clean
	
