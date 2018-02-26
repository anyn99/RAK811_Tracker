################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/radio/sx1276/sx1276.c 

OBJS += \
./src/radio/sx1276/sx1276.o 

C_DEPS += \
./src/radio/sx1276/sx1276.d 


# Each subdirectory must supply rules for building sources it contributes
src/radio/sx1276/%.o: ../src/radio/sx1276/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_DEBUGGER -DHARD_FAULT_HANDLER_ENABLED -DUSE_HAL_DRIVER -DSTM32L151CBU6 -DREGION_EU868 -DSTM32L151xB -DTRACE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l1xx" -I../src/STM32_USB_Device_Library/Class/DFU/Inc -I../src/STM32_USB_Device_Library/Core/Inc -I../src/usb/cdc/inc -I../src/usb/dfu/inc -I../src/STM32_USB_Device_Library/Class/CDC/Inc -I../src/system -I../src/radio -I../src/peripherals -I../src/mac -I../src/system/crypto -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


