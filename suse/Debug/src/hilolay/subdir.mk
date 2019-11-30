################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hilolay/hilolay_alumnos.c \
../src/hilolay/hilolay_internal.c 

OBJS += \
./src/hilolay/hilolay_alumnos.o \
./src/hilolay/hilolay_internal.o 

C_DEPS += \
./src/hilolay/hilolay_alumnos.d \
./src/hilolay/hilolay_internal.d 


# Each subdirectory must supply rules for building sources it contributes
src/hilolay/%.o: ../src/hilolay/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


