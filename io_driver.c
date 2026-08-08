/*
 * io_driver.c
 *
 *  Created on: 26 Tem 2026
 *      Author: Mansu
 */
#include "io_driver.h"



void IO_Initialization(IO_Info_t *ioInfo)
{
	/************** Input Initialization *************/

	ioInfo->inputs_Info.User_Button.GPIOx		= User_Button_GPIO_Port;	// Port Bilgisi Atama
	ioInfo->inputs_Info.User_Button.GPIO_Pin		= User_Button_Pin;			//Pin Bilgisi Atama
	ioInfo->inputs_Info.User_Button.numOfInput	= 0;						// pin numarası // her yeni pin atamasında bu değerin arttırılması gereklidir
	ioInfo->inputs_Info.User_Button.currentState = GPIO_PIN_RESET;			//ilk durum bilgisi
	ioInfo->inputs_Info.User_Button.lastState	= GPIO_PIN_RESET;			// son durum bilgisi
	ioInfo->inputs_Info.User_Button.inputStatus 	= Input_Status_LOW;			//karar verilmis dijital giris durumu low olarak ayarlandı
	ioInfo->inputs_Info.User_Button.currentTime	= 0;						//debounce algoritması baslangic zamani
	ioInfo->inputs_Info.User_Button.debounceTime = DEBOUNCE_TIME;			//DEBOUNB süresi ayarlama


	/************** Output Initialization *************/

	ioInfo->outputs_Info.ledGreen.GPIOx			= User_Led0_GPIO_Port;
	ioInfo->outputs_Info.ledGreen.GPIO_Pin		= User_Led0_Pin;
	ioInfo->outputs_Info.ledGreen.pinStates		= GPIO_PIN_RESET;

	ioInfo->outputs_Info.ledBlue.GPIOx			= User_Led2_GPIO_Port;
	ioInfo->outputs_Info.ledBlue.GPIO_Pin		= User_Led2_Pin;
	ioInfo->outputs_Info.ledBlue.pinStates		= GPIO_PIN_RESET;

	ioInfo->outputs_Info.ledYellow.GPIOx		= User_Led3_GPIO_Port;
	ioInfo->outputs_Info.ledYellow.GPIO_Pin		= User_Led3_Pin;
	ioInfo->outputs_Info.ledYellow.pinStates	= GPIO_PIN_RESET;

	ioInfo->outputs_Info.ledRed.GPIOx			= User_Led4_GPIO_Port;
	ioInfo->outputs_Info.ledRed.GPIO_Pin		= User_Led4_Pin;
	ioInfo->outputs_Info.ledRed.pinStates		= GPIO_PIN_RESET;
}



void IO_Status_Control(IO_Info_t *ioInfo)
{
	IO_Output_Control(&ioInfo->outputs_Info.ledGreen);
	IO_Output_Control(&ioInfo->outputs_Info.ledBlue);
	IO_Output_Control(&ioInfo->outputs_Info.ledYellow);
	IO_Output_Control(&ioInfo->outputs_Info.ledRed);

	IO_Input_Control_With_Debounce(&ioInfo->inputs_Info.User_Button);
}

void IO_Input_Control_With_Debounce(Input_State_t *inputStates)
{
	inputStates->currentState = HAL_GPIO_ReadPin(inputStates->GPIOx, inputStates->GPIO_Pin);

	if(inputStates->currentState == GPIO_PIN_RESET)
	{
		if(inputStates->inputStatus == Input_Status_LOW)
		{
			if(inputStates->currentState != inputStates->lastState)
			{
				inputStates->lastState = inputStates->currentTime;
				inputStates->currentTime = HAL_GetTick();
			}

			if((HAL_GetTick() - inputStates->currentTime) >= DEBOUNCE_TIME)
			{
				inputStates->inputStatus = Input_Status_HIGH;
			}
		}
		else
		{
			//ZATEN HIGH
		}

	}
	else
	{
		inputStates->lastState = inputStates->currentState;
		inputStates->inputStatus = Input_Status_LOW;
	}
}


void IO_Output_Control(Output_States_t *outputStates)
{
	HAL_GPIO_WritePin(outputStates->GPIOx, outputStates->GPIO_Pin, outputStates->pinStates);
}

