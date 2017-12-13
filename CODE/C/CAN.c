#include "stm32f10x_conf.h"
#include "CAN.h"

unsigned char CAN_Send_Success_Flag = 0;
extern unsigned int RH,Tempr;
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;  tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
    GPIO_InitTypeDef        GPIO_InitStructure; 
    CAN_InitTypeDef         CAN_InitStructure;
    CAN_FilterInitTypeDef   CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
    NVIC_InitTypeDef        NVIC_InitStructure;
#endif

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能PORTA时钟                                                                
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);    //使能CAN1时钟  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化IO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //初始化IO
    //CAN单元设置
    CAN_InitStructure.CAN_TTCM=DISABLE;         //非时间触发通信模式  
    CAN_InitStructure.CAN_ABOM=DISABLE;         //软件自动离线管理   
    CAN_InitStructure.CAN_AWUM=DISABLE;         //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN_InitStructure.CAN_NART=ENABLE;          //禁止报文自动传送 
    CAN_InitStructure.CAN_RFLM=DISABLE;         //报文不锁定,新的覆盖旧的  
    CAN_InitStructure.CAN_TXFP=DISABLE;         //优先级由报文标识符决定 
    CAN_InitStructure.CAN_Mode= mode;          //模式设置： mode:0,普通模式;1,回环模式; 
    //设置波特率
    CAN_InitStructure.CAN_SJW=tsjw;             //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq    CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
    CAN_InitStructure.CAN_BS1=tbs1;             //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
    CAN_InitStructure.CAN_BS2=tbs2;             //Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~    CAN_BS2_8tq
    CAN_InitStructure.CAN_Prescaler=brp;        //分频系数(Fdiv)为brp+1  
    CAN_Init(CAN1, &CAN_InitStructure);         //初始化CAN1 

    CAN_FilterInitStructure.CAN_FilterNumber=0; //过滤器0
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;   //屏蔽位模式
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;  //32位宽 
    CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;    //32位ID
    CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//激活过滤器0
    CAN_FilterInit(&CAN_FilterInitStructure);           //滤波器初始化

#if CAN_RX0_INT_ENABLE 
    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);              //FIFO0消息挂号中断允许.            

    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
    return 0;
} 

#if CAN_RX0_INT_ENABLE  //使能RX0中断
//中断服务函数                
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CanRxMsg RxMessage;
    int i=0;
  CAN_Receive(CAN1, 0, &RxMessage);
    for(i=0;i<8;i++)
    printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)   
//len:数据长度(最大为8)                     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//       其他,失败;
u8 Can_Send_Msg(u8* msg,u8 len)
{   
    u8 mbox;
    u16 i=0;
    CanTxMsg TxMessage;
	
    TxMessage.StdId= 0x0528;           // 标准标识符 
    TxMessage.ExtId= 0x0528;           // 设置扩展标示符 
    TxMessage.IDE=CAN_Id_Standard;  // 标准帧
    TxMessage.RTR=CAN_RTR_Data;     // 数据帧
    TxMessage.DLC=len;              // 要发送的数据长度
    for(i=0;i<len;i++)
   	 TxMessage.Data[i]=msg[i];                     
    mbox= CAN_Transmit(CAN1, &TxMessage);   
    i=0; 
    while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++; //等待发送结束
    if(i>=0XFFF)
		return 0;
    return 1;    
}
//can口接收数据查询
//buf:数据缓存区;     
//返回值:0,无数据被收到;
//       其他,接收的数据长度;
u8 Can_Receive_Msg(u8 *buf)
{                  
    u32 i;
    CanRxMsg RxMessage;
	
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;     //没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据 
    for(i=0;i<8;i++)
    	buf[i]=RxMessage.Data[i];  
    return RxMessage.DLC;   
}
void CAN_data_read_IMS_MESS(unsigned char type,unsigned int data)
{
	unsigned char TxBuf[8];

	TxBuf[0] = LocalID >> 8;
	TxBuf[1] = LocalID & 0x00ff;
	TxBuf[2] = type;
	TxBuf[3] = COMMAND_BYTE_READ;
	TxBuf[4] = data / 1000;
	TxBuf[5] = data % 1000 / 100;
	TxBuf[6] = data % 100 / 10;
	TxBuf[7] = data % 10;

	if(Can_Send_Msg(TxBuf,8))
	{
		CAN_Send_Success_Flag = 1;
	}
	else
	{
		
	}
		
	
}
void CAN_data_read_process(unsigned char *CAN_data)
{	

	switch(CAN_data[2])											//读数据
	{

		case DATA_TYPE_WATER_TEMPERATURE:						// 水温	0x03
		case DATA_TYPE_VOLTAGE:									// 电压 0x04
		case DATA_TYPE_CURRENT:									// 电流	0x05

			break;
		case DATA_TYPE_TEMPERATURE:							// 温度
			CAN_data_read_IMS_MESS(DATA_TYPE_TEMPERATURE,Tempr);
			break;
		case DATA_TYPE_HUMIDTY:								// 湿度
			CAN_data_read_IMS_MESS(DATA_TYPE_HUMIDTY,RH);
			break;
		

		default:

			break;
	}
}

void CAN_Receive_Process(void)
{
	unsigned char RXD_Len;
	unsigned char RxBuf[8];
	unsigned int RxID;

	
	RXD_Len = Can_Receive_Msg(RxBuf);
	if(RXD_Len)
	{
		RxID = RxBuf[0]  * 256 + RxBuf[1];
		if(RxID == LocalID)
		{
			switch(RxBuf[3])
			{
				case COMMAND_BYTE_READ:					/* 读数据*/
					CAN_data_read_process(RxBuf);				
					break;

				case COMMAND_BYTE_INQUIRE:				/* 查询设备状态*/
					//CAN_data_inquire_device_state(CAN_data_buf);
					break;

				case COMMAND_BYTE_WRITE:				/* 写参数*/
					//CAN_data_write_process(CAN_data_buf);
					break;

				case COMMAND_BYTE_CONTROL:				/* 控制*/
					//CAN_control_process(CAN_data_buf);
					break;

				case COMMAND_BYTE_ALARM_UNACTIVE:		/* 报警*/
					//CAN_alarm_process(CAN_data_buf);
					break;

				default:

					break;
			}
		}
	}
}

void CAN_LED_Process(void)
{
	static unsigned char CAN_Send_Success_Count = 0;
	static unsigned char Send_Success_Flash_Count = 0;
	
	if(CAN_Send_Success_Flag)
	{
		if(CAN_Send_Success_Count)
		{
			CAN_Send_Success_Count = 0;
			GPIO_ResetBits(GPIOB, GPIO_Pin_9);
		}
		else
		{
			CAN_Send_Success_Count = 1;
			GPIO_SetBits(GPIOB,GPIO_Pin_9);
		}
		Send_Success_Flash_Count++;
		if(Send_Success_Flash_Count > 20)
		{
			Send_Success_Flash_Count = 0;
			CAN_Send_Success_Flag = 0;
			GPIO_SetBits(GPIOB, GPIO_Pin_9);
		}
	}
}
