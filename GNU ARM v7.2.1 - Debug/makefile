################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include driver/subdir.mk
-include CMSIS/EFM32TG11B/subdir.mk
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
all: safebattery01.axf

# Tool invocations
safebattery01.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m0plus -mthumb -T "safebattery01.ld" -Xlinker --gc-sections -Xlinker -Map="safebattery01.map" --specs=nano.specs -o safebattery01.axf "./src/main.o" "./emlib/em_acmp.o" "./emlib/em_adc.o" "./emlib/em_aes.o" "./emlib/em_assert.o" "./emlib/em_burtc.o" "./emlib/em_can.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_cryotimer.o" "./emlib/em_crypto.o" "./emlib/em_csen.o" "./emlib/em_dac.o" "./emlib/em_dbg.o" "./emlib/em_dma.o" "./emlib/em_ebi.o" "./emlib/em_emu.o" "./emlib/em_eusart.o" "./emlib/em_gpcrc.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_iadc.o" "./emlib/em_idac.o" "./emlib/em_int.o" "./emlib/em_lcd.o" "./emlib/em_ldma.o" "./emlib/em_lesense.o" "./emlib/em_letimer.o" "./emlib/em_leuart.o" "./emlib/em_mpu.o" "./emlib/em_msc.o" "./emlib/em_opamp.o" "./emlib/em_pcnt.o" "./emlib/em_pdm.o" "./emlib/em_prs.o" "./emlib/em_qspi.o" "./emlib/em_rmu.o" "./emlib/em_rtc.o" "./emlib/em_rtcc.o" "./emlib/em_se.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./emlib/em_vcmp.o" "./emlib/em_vdac.o" "./emlib/em_wdog.o" "./driver/adcdrv.o" "./driver/candrv.o" "./driver/crc.o" "./driver/i2cdrv.o" "./driver/ltc4151drv.o" "./driver/mainctrl.o" "./driver/timer.o" "./CMSIS/EFM32TG11B/startup_efm32tg11b.o" "./CMSIS/EFM32TG11B/system_efm32tg11b.o" -lm -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: safebattery01.hex'
	arm-none-eabi-objcopy -O ihex "safebattery01.axf" "safebattery01.hex"
	@echo ' '

	@echo 'Building bin file: safebattery01.bin'
	arm-none-eabi-objcopy -O binary "safebattery01.axf" "safebattery01.bin"
	@echo ' '

	@echo 'Building s37 file: safebattery01.s37'
	arm-none-eabi-objcopy -O srec "safebattery01.axf" "safebattery01.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "safebattery01.axf" -A
	@echo ' '

	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) safebattery01.axf
	-@echo ' '

post-build:
	-arm-none-eabi-objdump -S safebattery01.axf > safebattery01.Stxt
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
