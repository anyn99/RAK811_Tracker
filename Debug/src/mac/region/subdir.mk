################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mac/region/Region.c \
../src/mac/region/RegionAS923.c \
../src/mac/region/RegionAU915.c \
../src/mac/region/RegionCN470.c \
../src/mac/region/RegionCN779.c \
../src/mac/region/RegionCommon.c \
../src/mac/region/RegionEU433.c \
../src/mac/region/RegionEU868.c \
../src/mac/region/RegionIN865.c \
../src/mac/region/RegionKR920.c \
../src/mac/region/RegionUS915-Hybrid.c \
../src/mac/region/RegionUS915.c 

OBJS += \
./src/mac/region/Region.o \
./src/mac/region/RegionAS923.o \
./src/mac/region/RegionAU915.o \
./src/mac/region/RegionCN470.o \
./src/mac/region/RegionCN779.o \
./src/mac/region/RegionCommon.o \
./src/mac/region/RegionEU433.o \
./src/mac/region/RegionEU868.o \
./src/mac/region/RegionIN865.o \
./src/mac/region/RegionKR920.o \
./src/mac/region/RegionUS915-Hybrid.o \
./src/mac/region/RegionUS915.o 

C_DEPS += \
./src/mac/region/Region.d \
./src/mac/region/RegionAS923.d \
./src/mac/region/RegionAU915.d \
./src/mac/region/RegionCN470.d \
./src/mac/region/RegionCN779.d \
./src/mac/region/RegionCommon.d \
./src/mac/region/RegionEU433.d \
./src/mac/region/RegionEU868.d \
./src/mac/region/RegionIN865.d \
./src/mac/region/RegionKR920.d \
./src/mac/region/RegionUS915-Hybrid.d \
./src/mac/region/RegionUS915.d 


# Each subdirectory must supply rules for building sources it contributes
src/mac/region/%.o: ../src/mac/region/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_DEBUGGER -DHARD_FAULT_HANDLER_ENABLED -DUSE_HAL_DRIVER -DSTM32L151CBU6 -DREGION_EU868 -DSTM32L151xB -DTRACE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l1xx" -I../src/STM32_USB_Device_Library/Class/DFU/Inc -I../src/STM32_USB_Device_Library/Core/Inc -I../src/usb/cdc/inc -I../src/usb/dfu/inc -I../src/STM32_USB_Device_Library/Class/CDC/Inc -I../src/system -I../src/radio -I../src/peripherals -I../src/mac -I../src/system/crypto -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


