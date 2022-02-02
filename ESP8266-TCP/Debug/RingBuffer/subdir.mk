################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RingBuffer/ring_buffer.c 

C_DEPS += \
./RingBuffer/ring_buffer.d 

OBJS += \
./RingBuffer/ring_buffer.o 


# Each subdirectory must supply rules for building sources it contributes
RingBuffer/ring_buffer.o: ../RingBuffer/ring_buffer.c RingBuffer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/topuz/STM32CubeIDE/workspace_1.6.0/ESP8266-TCP-IP/ESP8266-TCP/ESP8266_Driver" -I"C:/Users/Topuz/STM32CubeIDE/workspace_1.6.0/ESP8266-TCP-IP/ESP8266-TCP/RingBuffer" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"RingBuffer/ring_buffer.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

