################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../code/MurTree/Utilities/file_reader.cpp \
../code/MurTree/Utilities/parameter_handler.cpp 

OBJS += \
./code/MurTree/Utilities/file_reader.o \
./code/MurTree/Utilities/parameter_handler.o 

CPP_DEPS += \
./code/MurTree/Utilities/file_reader.d \
./code/MurTree/Utilities/parameter_handler.d 


# Each subdirectory must supply rules for building sources it contributes
code/MurTree/Utilities/%.o: ../code/MurTree/Utilities/%.cpp code/MurTree/Utilities/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -Ofast -O3 -std=c++14 -DNDEBUG -Wall -c -fmessage-length=0 -flto -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


