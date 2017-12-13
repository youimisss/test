#include "stm32f10x_conf.h"
#include "stdio.h"


//PA9 ----UART_TX
//PA10 ----UART_RX
/********************************************************************************
ʹ��printf���ڳ�����Բ��裺
1����ѡUse MicroLIB
2�������а���USART.H USART.C�ļ�
3��main�����а���stdio.hͷ�ļ�
4��main�����жԴ��ڽ��г�ʼ����USART_INIT();
5�����ļ�δ�򿪴����ж��µ���ͨ��
********************************************************************************/
void USART_INIT (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/*ʹ�ܴ���1�ʹ���1ʹ�õ�GPIOʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);

	/*ʹ�ܴ���1ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/***PA9=TX �������***/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/***PA10=RX ��������***/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b ;
	USART_Init(USART1,&USART_InitStructure);
	
  USART_Cmd(USART1, ENABLE);	 /* ʹ�ܴ���1 */
	
}

//void NVIC_Configuration(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	
//	NVIC_Init(&NVIC_InitStructure);
//	
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  /*ʹ�ܴ���1�Ľ����ж�*/
////  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	  /*ʹ�ܴ���1�ķ����ж�*/
//	
//}

// ��������
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (unsigned char) ch);// USART1 ???? USART2 ?
	while (!(USART1->SR & USART_FLAG_TXE));
	return (ch);
}
// ��������
int GetKey (void)  
{
	while (!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1FF));
}
