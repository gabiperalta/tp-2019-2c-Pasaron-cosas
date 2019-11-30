################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funcionesSuse.c \
../src/hilolay_alumnos.c \
../src/servidor.c \
../src/suse.c 

OBJS += \
./src/funcionesSuse.o \
./src/hilolay_alumnos.o \
./src/servidor.o \
./src/suse.o 

C_DEPS += \
./src/funcionesSuse.d \
./src/hilolay_alumnos.d \
./src/servidor.d \
./src/suse.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2019-2c-Pasaron-cosas/biblioteca" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


