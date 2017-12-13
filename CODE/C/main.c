#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include "stdint.h"
#include "stm32f10x_conf.h"
#include "Uart.h"
#include "Sensor.h"
#include "CAN.h"


#define OS_TICKS_PER_SEC       1000    /* Set the number of ticks in one second                        */

unsigned char Timer_1ms_bit = 0;
unsigned char Timer_100ms_bit = 0;
unsigned char Timer_500ms_bit = 0;
unsigned char Timer_1s_bit = 0;

unsigned int Timer_1ms_count = 0;
unsigned int Timer_100ms_count = 0;
unsigned int Timer_500ms_count = 0;
unsigned int Timer_1s_count = 0;

extern unsigned int RH,Tempr;
extern unsigned char CAN_Send_Success_Flag;

void LEDInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9);
}
void SysTick_init(void)
{
	SysTick_Config(SystemCoreClock/OS_TICKS_PER_SEC);
}
int main(void)
{
	static unsigned char SenSor_Read_Count = 0;
	static unsigned char LED_Run_Count = 0;
	
	
	
	LEDInit();
	USART_INIT();
	SensorInit();
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,20,CAN_Mode_Normal);//CAN普通模式初始化, 波特率100Kbps 
	SysTick_init();
	
	while(1)
	{
		if(Timer_1ms_bit)
		{
			Timer_1ms_bit = 0;
			CAN_Receive_Process();
			
		}
		if(Timer_100ms_bit)
		{
			Timer_100ms_bit = 0;
			CAN_LED_Process();
		}
		if(Timer_500ms_bit)
		{
			Timer_500ms_bit = 0;
			SenSor_Read_Count++;
			if(SenSor_Read_Count == 5)
			{
				SenSor_Read_Count = 0;
				Read_Sensor_Data();
				printf("RH = %4.2f%%,Temperature = %4.2fC\r\n",(float)RH/100,(float)Tempr/100);
			}
			
		}
		if(Timer_1s_bit)
		{
			Timer_1s_bit = 0;
			if(LED_Run_Count)
			{
				LED_Run_Count = 0;
				GPIO_ResetBits(GPIOB, GPIO_Pin_8);
			}
			else
			{
				LED_Run_Count = 1;
				GPIO_SetBits(GPIOB,GPIO_Pin_8);
			}
		}

	}

}


void SysTick_Handler(void)
{
	Timer_1ms_bit = 1;
	if(Timer_1ms_count == 99)
	{
		Timer_1ms_count = 0;
		Timer_100ms_bit = 1;
		if(Timer_100ms_count == 4)
		{
			Timer_100ms_count = 0;
			Timer_500ms_bit = 1;
			if(Timer_500ms_count == 1)
			{
				Timer_500ms_count = 0;
				Timer_1s_bit = 1;
			}
			else
				Timer_500ms_count++;
		}
		else
			Timer_100ms_count++;
	}
	else
		Timer_1ms_count++;
}
