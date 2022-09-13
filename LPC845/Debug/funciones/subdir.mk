################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../funciones/Aes.c \
../funciones/CryptoUtilities.c \
../funciones/Flash.c \
../funciones/MainMe.c \
../funciones/Rsa.c \
../funciones/Storage.c \
../funciones/TramaMe.c \
../funciones/Utilities.c 

OBJS += \
./funciones/Aes.o \
./funciones/CryptoUtilities.o \
./funciones/Flash.o \
./funciones/MainMe.o \
./funciones/Rsa.o \
./funciones/Storage.o \
./funciones/TramaMe.o \
./funciones/Utilities.o 

C_DEPS += \
./funciones/Aes.d \
./funciones/CryptoUtilities.d \
./funciones/Flash.d \
./funciones/MainMe.d \
./funciones/Rsa.d \
./funciones/Storage.d \
./funciones/TramaMe.d \
./funciones/Utilities.d 


# Each subdirectory must supply rules for building sources it contributes
funciones/%.o: ../funciones/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_LPC845M301JBD48 -DCPU_LPC845M301JBD48_cm0plus -DMCUXPRESSO_SDK -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\board" -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\funciones" -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\headers" -I"G:\My Drive\MisArchivos\UTN\Informatica II\C\Librerias\infotroniclibrary_lpc845\inc" -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\source" -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\utilities" -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\drivers" -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\device" -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\component\uart" -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\CMSIS" -I"C:\Users\matuz\git\eid-infoii\LPC845_EID\lpc845breakout\driver_examples\iap\iap_flash" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


