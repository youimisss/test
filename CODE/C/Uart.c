#include "stm32f10x_conf.h"
#include "stdio.h"


//PA9 ----UART_TX
//PA10 ----UART_RX
/********************************************************************************
使用printf用于程序调试步骤：
1、勾选Use MicroLIB
2、工程中包含USART.H USART.C文件
3、main函数中包含stdio.h头文件
4、main函数中对串口进行初始化：USART_INIT();
5、该文件未打开串口中断下调试通过
********************************************************************************/
void USART_INIT (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/*使能串口1和串口1使用的GPIO时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);

	/*使能串口1时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/***PA9=TX 推挽输出***/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/***PA10=RX 上拉输入***/
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
	
  USART_Cmd(USART1, ENABLE);	 /* 使能串口1 */
	
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
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  /*使能串口1的接收中断*/
////  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	  /*使能串口1的发送中断*/
//	
//}

// 发送数据
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (unsigned char) ch);// USART1 ???? USART2 ?
	while (!(USART1->SR & USART_FLAG_TXE));
	return (ch);
}
// 接收数据
int GetKey (void)  
{
	while (!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1FF));
}
