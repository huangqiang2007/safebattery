################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver/adcdrv.c \
../driver/candrv.c \
../driver/crc.c \
../driver/i2cdrv.c \
../driver/ltc4151drv.c \
../driver/mainctrl.c \
../driver/timer.c 

OBJS += \
./driver/adcdrv.o \
./driver/candrv.o \
./driver/crc.o \
./driver/i2cdrv.o \
./driver/ltc4151drv.o \
./driver/mainctrl.o \
./driver/timer.o 

C_DEPS += \
./driver/adcdrv.d \
./driver/candrv.d \
./driver/crc.d \
./driver/i2cdrv.d \
./driver/ltc4151drv.d \
./driver/mainctrl.d \
./driver/timer.d 


# Each subdirectory must supply rules for building sources it contributes
driver/adcdrv.o: ../driver/adcdrv.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DEFM32TG11B120F128GQ48=1' -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"D:\0WanHaiWork\work20XX\FILE20190916safebattery\safebattery01\include" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32TG11B/Include" -Og -Wall -c -fmessage-length=0 -fstack-usage -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"driver/adcdrv.d" -MT"driver/adcdrv.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

driver/candrv.o: ../driver/candrv.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DEFM32TG11B120F128GQ48=1' -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"D:\0WanHaiWork\work20XX\FILE20190916safebattery\safebattery01\include" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32TG11B/Include" -Og -Wall -c -fmessage-length=0 -fstack-usage -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"driver/candrv.d" -MT"driver/candrv.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

driver/crc.o: ../driver/crc.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DEFM32TG11B120F128GQ48=1' -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"D:\0WanHaiWork\work20XX\FILE20190916safebattery\safebattery01\include" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32TG11B/Include" -Og -Wall -c -fmessage-length=0 -fstack-usage -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"driver/crc.d" -MT"driver/crc.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

driver/i2cdrv.o: ../driver/i2cdrv.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DEFM32TG11B120F128GQ48=1' -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"D:\0WanHaiWork\work20XX\FILE20190916safebattery\safebattery01\include" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32TG11B/Include" -Og -Wall -c -fmessage-length=0 -fstack-usage -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"driver/i2cdrv.d" -MT"driver/i2cdrv.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

driver/ltc4151drv.o: ../driver/ltc4151drv.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DEFM32TG11B120F128GQ48=1' -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"D:\0WanHaiWork\work20XX\FILE20190916safebattery\safebattery01\include" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32TG11B/Include" -Og -Wall -c -fmessage-length=0 -fstack-usage -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"driver/ltc4151drv.d" -MT"driver/ltc4151drv.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

driver/mainctrl.o: ../driver/mainctrl.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DEFM32TG11B120F128GQ48=1' -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"D:\0WanHaiWork\work20XX\FILE20190916safebattery\safebattery01\include" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32TG11B/Include" -Og -Wall -c -fmessage-length=0 -fstack-usage -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"driver/mainctrl.d" -MT"driver/mainctrl.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

driver/timer.o: ../driver/timer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -std=c99 '-DEFM32TG11B120F128GQ48=1' -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"D:\0WanHaiWork\work20XX\FILE20190916safebattery\safebattery01\include" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -I"D:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32TG11B/Include" -Og -Wall -c -fmessage-length=0 -fstack-usage -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"driver/timer.d" -MT"driver/timer.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


