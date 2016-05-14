################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../peripherals/src/I2C.c \
../peripherals/src/clock.c \
../peripherals/src/dht.c \
../peripherals/src/ds1307.c \
../peripherals/src/hardware.c \
../peripherals/src/lcd.c \
../peripherals/src/mynixie.c \
../peripherals/src/nrf24l01.c \
../peripherals/src/spi.c \
../peripherals/src/uart.c 

OBJS += \
./peripherals/src/I2C.o \
./peripherals/src/clock.o \
./peripherals/src/dht.o \
./peripherals/src/ds1307.o \
./peripherals/src/hardware.o \
./peripherals/src/lcd.o \
./peripherals/src/mynixie.o \
./peripherals/src/nrf24l01.o \
./peripherals/src/spi.o \
./peripherals/src/uart.o 

C_DEPS += \
./peripherals/src/I2C.d \
./peripherals/src/clock.d \
./peripherals/src/dht.d \
./peripherals/src/ds1307.d \
./peripherals/src/hardware.d \
./peripherals/src/lcd.d \
./peripherals/src/mynixie.d \
./peripherals/src/nrf24l01.d \
./peripherals/src/spi.d \
./peripherals/src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
peripherals/src/%.o: ../peripherals/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\Users\desLoges\Documents\Hardware\AVR\NixieSensors_v1\peripherals/inc" -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega64 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


