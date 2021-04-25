#include "dht11.h"


void delay_us(unsigned char n) 
{
for(;n>0;n--)
{
asm("nop"); //在STM8里面，16M晶振，_nop_() 延时了 333ns
asm("nop");
asm("nop");
asm("nop");
}
}
/*
 * ��������DHT11_GPIO_Config
 * ����  ������DHT11�õ���I/O��
 * ����  ����
 * ���  ����
 */

void DHT11_GPIO_Config(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStruct;

	/*����DHT11_PORT������ʱ��*/
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*ѡ��Ҫ���Ƶ�DHT11_PORT����*/
  	GPIO_InitStruct.Pin = DHT11_PIN;

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  

	/*������������Ϊ50MHz �е㲻һ����������*/   
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	/*���ÿ⺯������ʼ��DHT11_PORT*/
  	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);	  

	/* ����GPIOA5	*/
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
}

/*
 * ��������DHT11_Mode_IPU
 * ����  ��ʹDHT11-DATA���ű�Ϊ��������ģʽ
 * ����  ����
 * ���  ����
 */
void DHT11_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStruct;

	  	/*ѡ��Ҫ���Ƶ�DHT11_PORT����*/	
	  GPIO_InitStruct.Pin = DHT11_PIN;

	   /*��������ģʽΪ��������ģʽ*/ 
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

	  /*���ÿ⺯������ʼ��DHT11_PORT*/
	  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/*
 * ��������DHT11_Mode_Out_PP
 * ����  ��ʹDHT11-DATA���ű�Ϊ�������ģʽ
 * ����  ����
 * ���  ����
 */
void DHT11_Mode_Out_PP(void)
{
 	 GPIO_InitTypeDef GPIO_InitStruct;

	/*ѡ��Ҫ���Ƶ�DHT11_PORT����*/
  	 GPIO_InitStruct.Pin = DHT11_PIN;

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 

	/*������������Ϊ50MHz */   
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	/*���ÿ⺯������ʼ��DHT11_PORT*/
  	HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/* 
 * ��DHT11��ȡһ���ֽڣ�MSB����
 */
uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		/*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/  
		while(DHT11_DATA_IN()==GPIO_PIN_RESET);

		/*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
		 *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
		 */
		// HAL_Delay(40); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��	
	   	delay_us(40);

		if(DHT11_DATA_IN()==GPIO_PIN_SET)/* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
		{
			/* �ȴ�����1�ĸߵ�ƽ���� */
			while(DHT11_DATA_IN()==GPIO_PIN_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ��1��MSB���� 
		}
		else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ��0��MSB����
		}
	}
	return temp;
}
/*
 * һ�����������ݴ���Ϊ40bit����λ�ȳ�
 * 8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
 */
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
	/*���ģʽ*/
	DHT11_Mode_Out_PP();
	/*��������*/
	DHT11_DATA_OUT(LOW);
	/*��ʱ18ms*/
	HAL_Delay(18);

	/*�������� ������ʱ30us*/
	DHT11_DATA_OUT(HIGH); 

	// HAL_Delay(30);   //��ʱ30us
	delay_us(30);

	/*������Ϊ���� �жϴӻ���Ӧ�ź�*/ 
	DHT11_Mode_IPU();

	/*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������*/   
	if(DHT11_DATA_IN()==GPIO_PIN_RESET)     
	{
		/*��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���*/  
		while(DHT11_DATA_IN()==GPIO_PIN_RESET);

		/*��ѯֱ���ӻ������� 80us �ߵ�ƽ �����źŽ���*/
		while(DHT11_DATA_IN()==GPIO_PIN_SET);

		/*��ʼ��������*/   
		DHT11_Data->humi_int= Read_Byte();

		DHT11_Data->humi_deci= Read_Byte();

		DHT11_Data->temp_int= Read_Byte();

		DHT11_Data->temp_deci= Read_Byte();

		DHT11_Data->check_sum= Read_Byte();


		/*��ȡ���������Ÿ�Ϊ���ģʽ*/
		DHT11_Mode_Out_PP();
		/*��������*/
		DHT11_DATA_OUT(HIGH);

		/*����ȡ�������Ƿ���ȷ*/
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return 1;
		else 
			return 0;
	}
	else
	{		
		return 2;
	}   
}

	  


/*************************************END OF FILE******************************/
