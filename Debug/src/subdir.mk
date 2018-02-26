################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/_write.c \
../src/adc-board.c \
../src/board.c \
../src/eeprom-board.c \
../src/gpio-board.c \
../src/gps-board.c \
../src/i2c-board.c \
../src/main.c \
../src/rtc-board.c \
../src/spi-board.c \
../src/sx1276-board.c \
../src/sysIrqHandlers.c \
../src/syscalls.c \
../src/uart-board.c \
../src/utilities.c 

OBJS += \
./src/_write.o \
./src/adc-board.o \
./src/board.o \
./src/eeprom-board.o \
./src/gpio-board.o \
./src/gps-board.o \
./src/i2c-board.o \
./src/main.o \
./src/rtc-board.o \
./src/spi-board.o \
./src/sx1276-board.o \
./src/sysIrqHandlers.o \
./src/syscalls.o \
./src/uart-board.o \
./src/utilities.o 

C_DEPS += \
./src/_write.d \
./src/adc-board.d \
./src/board.d \
./src/eeprom-board.d \
./src/gpio-board.d \
./src/gps-board.d \
./src/i2c-board.d \
./src/main.d \
./src/rtc-board.d \
./src/spi-board.d \
./src/sx1276-board.d \
./src/sysIrqHandlers.d \
./src/syscalls.d \
./src/uart-board.d \
./src/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_DEBUGGER -DHARD_FAULT_HANDLER_ENABLED -DUSE_HAL_DRIVER -DSTM32L151CBU6 -DREGION_EU868 -DSTM32L151xB -DTRACE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l1xx" -I../src/STM32_USB_Device_Library/Class/DFU/Inc -I../src/STM32_USB_Device_Library/Core/Inc -I../src/usb/cdc/inc -I../src/usb/dfu/inc -I../src/STM32_USB_Device_Library/Class/CDC/Inc -I../src/system -I../src/radio -I../src/peripherals -I../src/mac -I../src/system/crypto -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


