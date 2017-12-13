#include "stm32f10x_conf.h"
#include "Sensor.h"

__asm int delay_us(unsigned long  usec)
{
	ALIGN
	PUSH.W {r1}		//2ʱ������
	MOV r1,#18		//1ʱ������
	MUL r0,r1		//1ʱ������
	SUB r0,#3		//1ʱ������
loop
	SUBS r0,#1			//1ʱ������
	BNE loop			//�����ת��Ϊ3�����ڣ�������ֻ��1������
	POP {r1}			//2ʱ������
	BX lr				//3��ʱ������
						//�ܹ���������Ϊ(usec*4)-4,�˴���4��Ҫ���ڵ������ô˺���������ʱ�����ڣ�����1ʱ�ӣ�BLX��ת3ʱ�ӣ�
}


void SensorInit(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //��1?��PORTG?������?�� 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				//PORTG.11 ��?������?3?
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_AF_OD; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOB,GPIO_Pin_12);    //��?3?1

}
unsigned char Data_Out(void)
{
	unsigned char i,datatemp;
	unsigned char data = 0;
	unsigned char k = 0;

	for(i = 0; i < 8; i++)
	{
		while(!SENSOR_DQ_IN);
		while(SENSOR_DQ_IN && (k < 250) )
		{
			delay_us(1);
			k++;
		}
		if(k >= 250)
			break;
		else if(k > 40)
			datatemp = 1;
		else
			datatemp = 0;
		k = 0;
		data <<= 1;
		data |= datatemp;
	}
	return data;
}

unsigned int RH,Tempr;

unsigned  int RH_Arr[6];
unsigned  int Tempr_Arr[6];

void Read_Sensor_Data(void)
{
//	static unsigned char i = 0;
//	unsigned char k;
	unsigned char retry=0;
	unsigned char tempH,tempL;
	unsigned char CS;
	unsigned int RHBak,TemprBak;
//	unsigned int sum = 0;
	
	SENSOR_IO_OUT();
	SENSOR_DQ_OUT = 0;
	delay_us(1000);
	SENSOR_DQ_OUT = 1;
	SENSOR_IO_IN();
	delay_us(25);
	while((!SENSOR_DQ_IN) && (retry < 90))
	{
		retry++;
		delay_us(1);
	}
	if(retry >= 90)
		return;//error return
	while((SENSOR_DQ_IN) && (retry < 90))
	{
		retry++;
		delay_us(1);
	}
	tempH = Data_Out();
	tempL = Data_Out();
	CS = tempH  + tempL;
	RHBak = tempH *256 + tempL;
	tempH = Data_Out();
	tempL = Data_Out();
	CS = CS + tempH  + tempL;
	TemprBak = tempH *256 + tempL;
	tempH = Data_Out();
	if(CS == tempH)
	{
		#if 1
		RH = RHBak * 10;
		Tempr  = TemprBak * 10;
		#else
		RH_Arr[i] = RHBak * 10;
		Tempr_Arr[i]  = TemprBak * 10;
		i++;
		if(i > 5)
			i = 0;
		#endif
	}
	#if 0
	sum = 0;
	for(k = 0; k < 6; k++)
	{
		sum += RH_Arr[k];
		RH = sum / 6;
	}
	sum = 0;
	for(k = 0; k < 6; k++)
	{
		
		sum += Tempr_Arr[k];
		Tempr = sum / 6;
	}
	#endif
}

