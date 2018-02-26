################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/peripherals/gpio-ioe.c \
../src/peripherals/lis3dh.c \
../src/peripherals/mag3110.c \
../src/peripherals/mma8451.c \
../src/peripherals/mpl3115.c \
../src/peripherals/pam7q.c \
../src/peripherals/sx1509.c \
../src/peripherals/sx9500.c 

OBJS += \
./src/peripherals/gpio-ioe.o \
./src/peripherals/lis3dh.o \
./src/peripherals/mag3110.o \
./src/peripherals/mma8451.o \
./src/peripherals/mpl3115.o \
./src/peripherals/pam7q.o \
./src/peripherals/sx1509.o \
./src/peripherals/sx9500.o 

C_DEPS += \
./src/peripherals/gpio-ioe.d \
./src/peripherals/lis3dh.d \
./src/peripherals/mag3110.d \
./src/peripherals/mma8451.d \
./src/peripherals/mpl3115.d \
./src/peripherals/pam7q.d \
./src/peripherals/sx1509.d \
./src/peripherals/sx9500.d 


# Each subdirectory must supply rules for building sources it contributes
src/peripherals/%.o: ../src/peripherals/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_DEBUGGER -DHARD_FAULT_HANDLER_ENABLED -DUSE_HAL_DRIVER -DSTM32L151CBU6 -DREGION_EU868 -DSTM32L151xB -DTRACE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l1xx" -I../src/STM32_USB_Device_Library/Class/DFU/Inc -I../src/STM32_USB_Device_Library/Core/Inc -I../src/usb/cdc/inc -I../src/usb/dfu/inc -I../src/STM32_USB_Device_Library/Class/CDC/Inc -I../src/system -I../src/radio -I../src/peripherals -I../src/mac -I../src/system/crypto -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


