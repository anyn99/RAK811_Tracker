################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/usb/dfu/src/usbd_conf.c \
../src/usb/dfu/src/usbd_desc.c \
../src/usb/dfu/src/usbd_dfu_flash.c 

OBJS += \
./src/usb/dfu/src/usbd_conf.o \
./src/usb/dfu/src/usbd_desc.o \
./src/usb/dfu/src/usbd_dfu_flash.o 

C_DEPS += \
./src/usb/dfu/src/usbd_conf.d \
./src/usb/dfu/src/usbd_desc.d \
./src/usb/dfu/src/usbd_dfu_flash.d 


# Each subdirectory must supply rules for building sources it contributes
src/usb/dfu/src/%.o: ../src/usb/dfu/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DSTM32L151xB -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l1xx" -I/include/system/crypto -I/include/mac -I/include/mac/region -I/include/peripherals -I/include/radio -I/include/radio/sx1272 -I/include/radio/sx1276 -I/include/usb -I/RAK811_ClassA/src/STM32_USB_Device_Library/Class/CDC/Inc -I../src/STM32_USB_Device_Library/Class/DFU/Inc -I../src/STM32_USB_Device_Library/Core/Inc -I../src/usb/cdc/inc -I../src/usb/dfu/inc -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


