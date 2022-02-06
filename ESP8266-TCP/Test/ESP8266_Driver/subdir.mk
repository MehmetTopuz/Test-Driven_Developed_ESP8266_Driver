################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ESP8266_Driver/esp8266.c 

C_DEPS += \
./ESP8266_Driver/esp8266.d 

OBJS += \
./ESP8266_Driver/esp8266.o 


# Each subdirectory must supply rules for building sources it contributes
ESP8266_Driver/esp8266.o: ../ESP8266_Driver/esp8266.c ESP8266_Driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Topuz/STM32CubeIDE/workspace_1.6.0/ESP8266-TCP-IP/ESP8266-TCP/RingBuffer" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"ESP8266_Driver/esp8266.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

