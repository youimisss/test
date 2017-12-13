#ifndef __CAN_H__
#define __CAN_H__


//#define CAN_RX0_INT_ENABLE

#define LocalID		0x1805

#define	COMMAND_BYTE_READ						'R'
#define	COMMAND_BYTE_INQUIRE					'I'
#define	COMMAND_BYTE_WRITE						'W'
#define	COMMAND_BYTE_CONTROL					'C'
#define	COMMAND_BYTE_ALARM_UNACTIVE			'L'
#define	COMMAND_BYTE_ALARM_ACTIVE				'E'
#define  COMMAND_BYTE_MANUAL_CONTROL			'M'	


//----------------------------------------------------------------------------------
#define	DATA_TYPE_WATER_PRESSURE				0x01		// 水压/TLC0834		0x01
#define	DATA_TYPE_WATER_REGIMEN					0x02		// 水情/TLC0834		0x02
#define	DATA_TYPE_WATER_TEMPERATURE			0x03		// 水温/TLC0834		0x03
#define	DATA_TYPE_VOLTAGE						0x04		// 电压/TLC0838		0x04
#define	DATA_TYPE_CURRENT						0x05		// 电流/TLC0838		0x05
#define	DATA_TYPE_TEMPERATURE					0x06		// 温度				0x06
#define	DATA_TYPE_HUMIDTY						0x07		// 湿度				0x07
#define	DATA_TYPE_CO2								0x08		// CO2				0x08
#define	DATA_TYPE_CO								0x09		// CO				0x09
#define	DATA_TYPE_O2								0x10		// O2				0x10
#define	DATA_TYPE_Rn								0x11		// 氡气				0x11
#define	DATA_TYPE_ADC0832							0x12		// 读TLC0832数据	0x12
#define	DATA_TYPE_ADC0834							0x13		// 读TLC0834数据	0x13
#define	DATA_TYPE_ADC0838							0x14		// 读TLC0838数据	0x14

#define	DATA_TYPE_ID								0x20		// 地址				0x20
#define	DATA_TYPE_CALIBRATION					0x21		// 误差校正值		0x21
#define	DATA_TYPE_ALARM_UP_LIMITS				0x22		// 报警上限值		0x22
#define	DATA_TYPE_ALARM_DOWN_LIMITS			0x23		// 报警下限值		0x23
#define	DATA_TYPE_ALARM_INTERVAL				0x24		// 主动报警间隔		0x24
#define	DATA_TYPE_CONTROL_VALVE					0x25		// 控制阀值	    	0x25
#define	DATA_TYPE_MODE_CONVERT_VALVE			0x26		// 模式转换控制阀值	0x26

#define	DATA_TYPE_READ							0x31		// 读备份			0x41
#define	DATA_TYPE_WRITE							0x32		// 写备份			0x42
//-----------------------------------------------------------------------------------

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);
void CAN_Receive_Process(void);
void CAN_LED_Process(void);

#endif
