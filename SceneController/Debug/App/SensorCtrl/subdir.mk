################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/SensorCtrl/DeviceCtrl.c \
../App/SensorCtrl/SceneElement.c \
../App/SensorCtrl/Transaction.c 

C_DEPS += \
./App/SensorCtrl/DeviceCtrl.d \
./App/SensorCtrl/SceneElement.d \
./App/SensorCtrl/Transaction.d 

OBJS += \
./App/SensorCtrl/DeviceCtrl.o \
./App/SensorCtrl/SceneElement.o \
./App/SensorCtrl/Transaction.o 


# Each subdirectory must supply rules for building sources it contributes
App/SensorCtrl/%.o: ../App/SensorCtrl/%.c App/SensorCtrl/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	aarch64-linux-gnu-gcc -I"/home/tct/git/repository/scenectrl/SceneController/Platform" -I/opt/libmodbus/install/include/modbus -I"/home/tct/git/repository/scenectrl/SceneController/Common" -I"/home/tct/git/repository/scenectrl/SceneController/App" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-App-2f-SensorCtrl

clean-App-2f-SensorCtrl:
	-$(RM) ./App/SensorCtrl/DeviceCtrl.d ./App/SensorCtrl/DeviceCtrl.o ./App/SensorCtrl/SceneElement.d ./App/SensorCtrl/SceneElement.o ./App/SensorCtrl/Transaction.d ./App/SensorCtrl/Transaction.o

.PHONY: clean-App-2f-SensorCtrl

