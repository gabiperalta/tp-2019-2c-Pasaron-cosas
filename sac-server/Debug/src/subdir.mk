################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funcionesSac.c \
../src/gestorDeMemoria.c \
../src/sac-server.c \
../src/servidor.c 

OBJS += \
./src/funcionesSac.o \
./src/gestorDeMemoria.o \
./src/sac-server.o \
./src/servidor.o 

C_DEPS += \
./src/funcionesSac.d \
./src/gestorDeMemoria.d \
./src/sac-server.d \
./src/servidor.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2019-2c-Pasaron-cosas/biblioteca" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


