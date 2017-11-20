################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../client_protocol.c \
../file_client.c \
../file_server.c \
../network.c \
../server_protocol.c 

OBJS += \
./client_protocol.o \
./file_client.o \
./file_server.o \
./network.o \
./server_protocol.o 

C_DEPS += \
./client_protocol.d \
./file_client.d \
./file_server.d \
./network.d \
./server_protocol.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


