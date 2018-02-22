################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/platform/system/src/interrupt/fsl_interrupt_manager.c 

OBJS += \
./SDK/platform/system/src/interrupt/fsl_interrupt_manager.o 

C_DEPS += \
./SDK/platform/system/src/interrupt/fsl_interrupt_manager.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/system/src/interrupt/%.o: ../SDK/platform/system/src/interrupt/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK64FN1M0VLL12" -D"FSL_RTOS_MQX" -D"PEX_MQX_KSDK" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/hal/inc" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/hal/src/sim/MK64F12" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/system/src/clock/MK64F12" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/system/inc" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/osa/inc" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/CMSIS/Include" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/devices" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/devices/MK64F12/include" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/utilities/src" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/utilities/inc" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/devices/MK64F12/startup" -I"/Users/mtayler/Documents/ceng455/serial_terminal/Generated_Code/SDK/platform/devices/MK64F12/startup" -I"/Users/mtayler/Documents/ceng455/serial_terminal/Sources" -I"/Users/mtayler/Documents/ceng455/serial_terminal/Generated_Code" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/drivers/inc" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/psp/cortex_m" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/psp/cortex_m/cpu" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/config/common" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/include" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/bsp" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/nio" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/nio/src" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/nio/fs" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/nio/drivers/nio_dummy" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/nio/drivers/nio_serial" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx/source/nio/drivers/nio_tty" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx_stdlib/source/include" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/rtos/mqx/mqx_stdlib/source/stdio" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/hal/src/uart" -I"/Users/mtayler/Documents/ceng455/serial_terminal/SDK/platform/drivers/src/uart" -I"/Users/mtayler/Documents/ceng455/serial_terminal/Generated_Code/SDK/rtos/mqx/config/board" -I"/Users/mtayler/Documents/ceng455/serial_terminal/Generated_Code/SDK/rtos/mqx/config/mcu" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


