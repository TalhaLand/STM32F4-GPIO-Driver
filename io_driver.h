/*
 * io_driver.h
 *
 *  Created on: 26 Tem 2026
 *      Author: Mansu
 */

#ifndef INC_IO_DRIVER_H_
#define INC_IO_DRIVER_H_
#include "main.h"

#define DEBOUNCE_TIME		100
typedef enum
{
	Input_Status_LOW		= 0,
	Input_Status_HIGH
}Input_Status_t;


typedef struct
{
	GPIO_TypeDef		*GPIOx;
	uint16_t			GPIO_Pin;
	uint16_t			numOfInput;

	GPIO_PinState		currentState;
	GPIO_PinState		lastState;

	Input_Status_t		inputStatus;

	uint32_t			currentTime;
	uint32_t			debounceTime;

}Input_State_t;

typedef struct
{
	Input_State_t		User_Button;

}Input_Info_t;

typedef struct
{
	GPIO_TypeDef		*GPIOx;
	uint16_t			GPIO_Pin;
	GPIO_PinState		pinStates;

}Output_States_t;

typedef struct
{
	Output_States_t		ledGreen;
	Output_States_t		ledBlue;
	Output_States_t		ledRed;
	Output_States_t		ledYellow;

}Output_Info_t;

typedef struct
{
	Input_Info_t		inputs_Info;
	Output_Info_t		outputs_Info;

}IO_Info_t;

void IO_Initialization(IO_Info_t *ioInfo);
void IO_Status_Control(IO_Info_t *ioInfo);
void IO_Input_Control_With_Debounce(Input_State_t *inputStates);
void IO_Output_Control(Output_States_t *outputStates);




#endif /* INC_IO_DRIVER_H_ */
