#include "stm32f10x_conf.h"
#include "CAN.h"

unsigned char CAN_Send_Success_Flag = 0;
extern unsigned int RH,Tempr;
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;  tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//������Ϊ:36M/((8+9+1)*4)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
    GPIO_InitTypeDef        GPIO_InitStructure; 
    CAN_InitTypeDef         CAN_InitStructure;
    CAN_FilterInitTypeDef   CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
    NVIC_InitTypeDef        NVIC_InitStructure;
#endif

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ��PORTAʱ��                                                                
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);    //ʹ��CAN1ʱ��  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //��ʼ��IO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //��ʼ��IO
    //CAN��Ԫ����
    CAN_InitStructure.CAN_TTCM=DISABLE;         //��ʱ�䴥��ͨ��ģʽ  
    CAN_InitStructure.CAN_ABOM=DISABLE;         //����Զ����߹���   
    CAN_InitStructure.CAN_AWUM=DISABLE;         //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    CAN_InitStructure.CAN_NART=ENABLE;          //��ֹ�����Զ����� 
    CAN_InitStructure.CAN_RFLM=DISABLE;         //���Ĳ�����,�µĸ��Ǿɵ�  
    CAN_InitStructure.CAN_TXFP=DISABLE;         //���ȼ��ɱ��ı�ʶ������ 
    CAN_InitStructure.CAN_Mode= mode;          //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; 
    //���ò�����
    CAN_InitStructure.CAN_SJW=tsjw;             //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq    CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
    CAN_InitStructure.CAN_BS1=tbs1;             //Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
    CAN_InitStructure.CAN_BS2=tbs2;             //Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~    CAN_BS2_8tq
    CAN_InitStructure.CAN_Prescaler=brp;        //��Ƶϵ��(Fdiv)Ϊbrp+1  
    CAN_Init(CAN1, &CAN_InitStructure);         //��ʼ��CAN1 

    CAN_FilterInitStructure.CAN_FilterNumber=0; //������0
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;   //����λģʽ
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;  //32λ�� 
    CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;    //32λID
    CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//���������0
    CAN_FilterInit(&CAN_FilterInitStructure);           //�˲�����ʼ��

#if CAN_RX0_INT_ENABLE 
    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);              //FIFO0��Ϣ�Һ��ж�����.            

    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
    return 0;
} 

#if CAN_RX0_INT_ENABLE  //ʹ��RX0�ж�
//�жϷ�����                
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  CanRxMsg RxMessage;
    int i=0;
  CAN_Receive(CAN1, 0, &RxMessage);
    for(i=0;i<8;i++)
    printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)   
//len:���ݳ���(���Ϊ8)                     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//       ����,ʧ��;
u8 Can_Send_Msg(u8* msg,u8 len)
{   
    u8 mbox;
    u16 i=0;
    CanTxMsg TxMessage;
	
    TxMessage.StdId= 0x0528;           // ��׼��ʶ�� 
    TxMessage.ExtId= 0x0528;           // ������չ��ʾ�� 
    TxMessage.IDE=CAN_Id_Standard;  // ��׼֡
    TxMessage.RTR=CAN_RTR_Data;     // ����֡
    TxMessage.DLC=len;              // Ҫ���͵����ݳ���
    for(i=0;i<len;i++)
   	 TxMessage.Data[i]=msg[i];                     
    mbox= CAN_Transmit(CAN1, &TxMessage);   
    i=0; 
    while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++; //�ȴ����ͽ���
    if(i>=0XFFF)
		return 0;
    return 1;    
}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;     
//����ֵ:0,�����ݱ��յ�;
//       ����,���յ����ݳ���;
u8 Can_Receive_Msg(u8 *buf)
{                  
    u32 i;
    CanRxMsg RxMessage;
	
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;     //û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ���� 
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

	switch(CAN_data[2])											//������
	{

		case DATA_TYPE_WATER_TEMPERATURE:						// ˮ��	0x03
		case DATA_TYPE_VOLTAGE:									// ��ѹ 0x04
		case DATA_TYPE_CURRENT:									// ����	0x05

			break;
		case DATA_TYPE_TEMPERATURE:							// �¶�
			CAN_data_read_IMS_MESS(DATA_TYPE_TEMPERATURE,Tempr);
			break;
		case DATA_TYPE_HUMIDTY:								// ʪ��
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
				case COMMAND_BYTE_READ:					/* ������*/
					CAN_data_read_process(RxBuf);				
					break;

				case COMMAND_BYTE_INQUIRE:				/* ��ѯ�豸״̬*/
					//CAN_data_inquire_device_state(CAN_data_buf);
					break;

				case COMMAND_BYTE_WRITE:				/* д����*/
					//CAN_data_write_process(CAN_data_buf);
					break;

				case COMMAND_BYTE_CONTROL:				/* ����*/
					//CAN_control_process(CAN_data_buf);
					break;

				case COMMAND_BYTE_ALARM_UNACTIVE:		/* ����*/
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
