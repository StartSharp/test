################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/SensorCtrl/DeviceCtrl.c 

C_DEPS += \
./App/SensorCtrl/DeviceCtrl.d 

OBJS += \
./App/SensorCtrl/DeviceCtrl.o 


# Each subdirectory must supply rules for building sources it contributes
App/SensorCtrl/%.o: ../App/SensorCtrl/%.c App/SensorCtrl/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	aarch64-linux-gnu-gcc -I"/home/tct/eclipse-workspace/SceneController/Platform" -I/opt/libmodbus/install/include/modbus -I"/home/tct/eclipse-workspace/SceneController/Common" -I"/home/tct/eclipse-workspace/SceneController/App" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-App-2f-SensorCtrl

clean-App-2f-SensorCtrl:
	-$(RM) ./App/SensorCtrl/DeviceCtrl.d ./App/SensorCtrl/DeviceCtrl.o

.PHONY: clean-App-2f-SensorCtrl

