################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Platform/AddXML/DeviceConf.c \
../Platform/AddXML/GP_Xml.c 

C_DEPS += \
./Platform/AddXML/DeviceConf.d \
./Platform/AddXML/GP_Xml.d 

OBJS += \
./Platform/AddXML/DeviceConf.o \
./Platform/AddXML/GP_Xml.o 


# Each subdirectory must supply rules for building sources it contributes
Platform/AddXML/%.o: ../Platform/AddXML/%.c Platform/AddXML/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	aarch64-linux-gnu-gcc -I"/home/tct/eclipse-workspace/SceneController/Platform" -I/opt/libmodbus/install/include/modbus -I"/home/tct/eclipse-workspace/SceneController/Common" -I"/home/tct/eclipse-workspace/SceneController/App" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-Platform-2f-AddXML

clean-Platform-2f-AddXML:
	-$(RM) ./Platform/AddXML/DeviceConf.d ./Platform/AddXML/DeviceConf.o ./Platform/AddXML/GP_Xml.d ./Platform/AddXML/GP_Xml.o

.PHONY: clean-Platform-2f-AddXML

