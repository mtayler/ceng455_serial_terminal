################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../SDK/platform/devices/MK64F12/startup/gcc/startup_MK64F12.S 

OBJS += \
./SDK/platform/devices/MK64F12/startup/gcc/startup_MK64F12.o 

S_UPPER_DEPS += \
./SDK/platform/devices/MK64F12/startup/gcc/startup_MK64F12.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/devices/MK64F12/startup/gcc/%.o: ../SDK/platform/devices/MK64F12/startup/gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -x assembler-with-cpp -D"FSL_RTOS_MQX" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/psp/cortex_m/core/M4" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/psp/cortex_m" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/include" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/config/common" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm" -I"/Users/mtayler/Documents/ceng455/serial_terminal/Generated_Code/SDK/rtos/mqx/config/mcu" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


