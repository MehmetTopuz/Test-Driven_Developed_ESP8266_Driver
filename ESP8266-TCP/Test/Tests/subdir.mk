################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Tests/main.cpp \
../Tests/tests.cpp 

OBJS += \
./Tests/main.o \
./Tests/tests.o 

CPP_DEPS += \
./Tests/main.d \
./Tests/tests.d 


# Each subdirectory must supply rules for building sources it contributes
Tests/main.o: ../Tests/main.cpp Tests/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DCPPUTEST_STD_CPP_LIB_DISABLED -DCPPUTEST_STD_C_LIB_DISABLED -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"C:/Users/topuz/STM32CubeIDE/workspace_1.6.0/ESP8266-TCP-IP/ESP8266-TCP/Tests/include" -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/topuz/STM32CubeIDE/workspace_1.6.0/ESP8266-TCP-IP/ESP8266-TCP/ESP8266_Driver" -I"C:/Users/Topuz/STM32CubeIDE/workspace_1.6.0/ESP8266-TCP-IP/ESP8266-TCP/RingBuffer" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -v -fstack-usage -MMD -MP -MF"Tests/main.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Tests/tests.o: ../Tests/tests.cpp Tests/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DCPPUTEST_STD_CPP_LIB_DISABLED -DCPPUTEST_STD_C_LIB_DISABLED -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"C:/Users/topuz/STM32CubeIDE/workspace_1.6.0/ESP8266-TCP-IP/ESP8266-TCP/Tests/include" -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/topuz/STM32CubeIDE/workspace_1.6.0/ESP8266-TCP-IP/ESP8266-TCP/ESP8266_Driver" -I"C:/Users/Topuz/STM32CubeIDE/workspace_1.6.0/ESP8266-TCP-IP/ESP8266-TCP/RingBuffer" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -v -fstack-usage -MMD -MP -MF"Tests/tests.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

