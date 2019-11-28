################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funcionesSuse.c \
../src/logs.c \
../src/servidor.c \
../src/suse.c 

OBJS += \
./src/funcionesSuse.o \
./src/logs.o \
./src/servidor.o \
./src/suse.o 

C_DEPS += \
./src/funcionesSuse.d \
./src/logs.d \
./src/servidor.d \
./src/suse.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


