################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include system/src/stm32l1xx/subdir.mk
-include system/src/newlib/subdir.mk
-include system/src/diag/subdir.mk
-include system/src/cortexm/subdir.mk
-include system/src/cmsis/subdir.mk
-include src/usb/cdc/src/subdir.mk
-include src/system/crypto/subdir.mk
-include src/system/subdir.mk
-include src/radio/sx1276/subdir.mk
-include src/peripherals/subdir.mk
-include src/mac/region/subdir.mk
-include src/mac/subdir.mk
-include src/STM32_USB_Device_Library/Core/Src/subdir.mk
-include src/STM32_USB_Device_Library/Class/CDC/Src/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
RAK811_ClassA.hex \

SECONDARY_SIZE += \
RAK811_ClassA.siz \


# All Target
all: RAK811_ClassA.elf secondary-outputs

# Tool invocations
RAK811_ClassA.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM GNU C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -T STM32L151XB_FLASH.ld -nostartfiles -Xlinker --gc-sections -L"../ldscripts" -Wl,-Map,"RAK811_ClassA.map" --specs=nano.specs -u _printf_float -o "RAK811_ClassA.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

RAK811_ClassA.hex: RAK811_ClassA.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "RAK811_ClassA.elf"  "RAK811_ClassA.hex"
	@echo 'Finished building: $@'
	@echo ' '

RAK811_ClassA.siz: RAK811_ClassA.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "RAK811_ClassA.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(CPP_DEPS) RAK811_ClassA.elf
	-@echo ' '

post-build:
	-@echo 'Copy HEX File'
	-cp /home/daniel/git/RAK811_ClassA/Debug/RAK811_ClassA.hex /media/sf_Share/
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents post-build

-include ../makefile.targets
