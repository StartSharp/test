################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/main.c 

C_DEPS += \
./App/main.d 

OBJS += \
./App/main.o 


# Each subdirectory must supply rules for building sources it contributes
App/%.o: ../App/%.c App/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	aarch64-linux-gnu-gcc -I"/home/tct/eclipse-workspace/SceneController/Platform" -I/opt/libmodbus/install/include/modbus -I"/home/tct/eclipse-workspace/SceneController/Common" -I"/home/tct/eclipse-workspace/SceneController/App" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-App

clean-App:
	-$(RM) ./App/main.d ./App/main.o

.PHONY: clean-App

