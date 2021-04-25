#ifndef __DHT11_H
#define	__DHT11_H

//PA4(A5)����DATA����
#include "main.h"
#define HIGH  1
#define LOW   0

#define DHT11_PIN     GPIO_PIN_4
#define DHT11_PORT		GPIOA 

//���κ꣬��������������һ��ʹ��,����ߵ�ƽ��͵�ƽ
#define DHT11_DATA_OUT(a)	if (a)	\
					HAL_GPIO_WritePin(DHT11_PORT,DHT11_PIN, GPIO_PIN_SET);\
					else		\
					HAL_GPIO_WritePin(DHT11_PORT,DHT11_PIN, GPIO_PIN_RESET)
 //��ȡ���ŵĵ�ƽ
#define  DHT11_DATA_IN()	   HAL_GPIO_ReadPin(DHT11_PORT,DHT11_PIN)

typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
		                 
}DHT11_Data_TypeDef;

void DHT11_GPIO_Config(void);
void DHT11_Mode_IPU(void);
void DHT11_Mode_Out_PP(void);
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);
uint8_t Read_Byte(void);

#endif /* __DHT11_H */







