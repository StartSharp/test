################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Platform/protocol/ModbusTCP/MobusTCPOps.c 

C_DEPS += \
./Platform/protocol/ModbusTCP/MobusTCPOps.d 

OBJS += \
./Platform/protocol/ModbusTCP/MobusTCPOps.o 


# Each subdirectory must supply rules for building sources it contributes
Platform/protocol/ModbusTCP/%.o: ../Platform/protocol/ModbusTCP/%.c Platform/protocol/ModbusTCP/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	aarch64-linux-gnu-gcc -I"/home/tct/eclipse-workspace/SceneController/Platform" -I/opt/libmodbus/install/include/modbus -I"/home/tct/eclipse-workspace/SceneController/Common" -I"/home/tct/eclipse-workspace/SceneController/App" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Platform-2f-protocol-2f-ModbusTCP

clean-Platform-2f-protocol-2f-ModbusTCP:
	-$(RM) ./Platform/protocol/ModbusTCP/MobusTCPOps.d ./Platform/protocol/ModbusTCP/MobusTCPOps.o

.PHONY: clean-Platform-2f-protocol-2f-ModbusTCP

