################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Platform/protocol/UDP/subdir.mk
-include Platform/protocol/TCP/subdir.mk
-include Platform/protocol/ModbusTCP/subdir.mk
-include Platform/Softtimer/subdir.mk
-include Platform/AddXML/subdir.mk
-include Common/subdir.mk
-include App/SensorCtrl/subdir.mk
-include App/CommandResolve/subdir.mk
-include App/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := SceneController
BUILD_ARTIFACT_EXTENSION :=
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: SceneController

# Tool invocations
SceneController: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	aarch64-linux-gnu-gcc -L/opt/libmodbus/install/lib -o "SceneController" $(OBJS) $(USER_OBJS) $(LIBS) -lpthread -lmodbus -lrt
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) SceneController
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
