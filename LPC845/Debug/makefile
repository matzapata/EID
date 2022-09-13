################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include source/subdir.mk
-include funciones/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include board/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: LPC845_EID.axf

# Tool invocations
LPC845_EID.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"G:\My Drive\MisArchivos\UTN\Informatica II\C\Librerias\infotroniclibrary_lpc845\Debug_Stick_LPC845" -L"G:\My Drive\MisArchivos\UTN\Informatica II\C\Librerias\infotroniclibrary_lpc845" -Xlinker -Map="LPC845_EID.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m0plus -mthumb -T "LPC845_EID_Debug.ld" -o "LPC845_EID.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) LPC845_EID.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "LPC845_EID.axf"; # arm-none-eabi-objcopy -v -O binary "LPC845_EID.axf" "LPC845_EID.bin" ; # checksum -p LPC845 -d "LPC845_EID.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
