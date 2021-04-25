/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

//#include "esp8266.h"
//lora
#include "platform.h"
#include "radio.h"
#include "sx1276-Hal.h"
#include "sx1276-LoRa.h"
#include "sx1276-LoRaMisc.h"

//dht11
#include "dht11.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BUFFERSIZE 10

#define SLAVENUM 0


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t RegVersion = 0;


// char request_num = -1;
// uint16_t sizeof_request_num = sizeof(request_num);
char data_frame_rev[BUFFERSIZE] = "";
char data_frame_send[BUFFERSIZE] = "";
uint16_t sizeof_data_frame = BUFFERSIZE;

DHT11_Data_TypeDef DHT11_Data;
// char wendu = 21;
// char shidu = 82;

//lora设备初始化的返回值
tRadioDriver *Radio = NULL;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void led_init()//LED初始化为三色
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);//LED_B
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);//LED_R
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);//LED_G
}
void led_send()//发送时：蓝灯
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}

void led_rev()//接收时：绿灯
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
}
void led_debug()//debug：红灯
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}


void slave_task()
{
	switch(Radio->Process())
	{
		case RF_RX_DONE:
			
			Radio->GetRxPacket(data_frame_rev, &sizeof_data_frame);
			printf("slave_task:rev  = -%d-\r\n",  data_frame_rev[0]);
			if(SLAVENUM == data_frame_rev[0])
			{
                led_send();//数据是我的，我就进入发送状态
				// while(!Read_DHT11(&DHT11_Data))printf("111\r\n");
				Read_DHT11(&DHT11_Data);
				data_frame_send[0] = SLAVENUM;
				data_frame_send[1] = DHT11_Data.temp_int;
				data_frame_send[2] = DHT11_Data.humi_int;
				// data_frame_send[3] = 0;
				HAL_Delay(1000);
				// printf("-%d-%d-%d-%d-\r\n", data_frame_send[0], data_frame_send[1], data_frame_send[2], data_frame_send[3]);
				// printf("-%s-\r\n", data_frame_send);
				Radio->SetTxPacket(data_frame_send, 3);
				// printf("-%s-\r\n", data_frame_send);
			}
			else
			{
				// HAL_Delay(1000);
				led_rev();//数据与我无关，不处理，继续回到接受状态
				Radio->StartRx();
			}
			break;
		case RF_TX_DONE:
				led_rev();//发完之后立马进入接受状态
				Radio->StartRx();
			break;
		default:
			break;
	}
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */
	

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

    led_init();
//    led_send();
	printf("Hello World!\r\n");
	SX1276Read( REG_LR_VERSION, &RegVersion );
	if(RegVersion != 0x12)
	{
	printf("get RegVersion error, RegVersion = %#x\r\n", RegVersion);
	}
	else
	{
	printf("get RegVersion success, RegVersion = %#x\r\n", RegVersion);
	}
	//sx1278初始化
	Radio = RadioDriverInit();
	Radio->Init();

    Radio->StartRx();
    printf("SLAVE%d start rev \r\n",SLAVENUM);
    

	while (1)
	{
	
      // Read_DHT11(&DHT11_Data);
      slave_task();
      
      // printf("%d\r\n", Read_DHT11(&DHT11_Data));
      // HAL_Delay(100);
      // printf("wendu = %d, shidu = %d\r\n", DHT11_Data.temp_int, DHT11_Data.humi_int);
      // HAL_Delay(2000);

      // led_debug();

      // led_init();
      // HAL_Delay(1000);
      // HAL_UART_Transmit(&huart2, ping_msg , strlen((const char *)ping_msg), 1000);

	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//int _write (int fd, char *pBuffer, int size) 
//{ 
//for (int i = 0; i < size; i++) 
//{ 
//while((USART1->SR&0X40)==0);//等待上一次串口数据发送完成 
//USART1->DR = (uint8_t) pBuffer[i]; //写DR,串口1将发送数据
//} 
//return size; 
//}

//重定向串口
int fputc(int c, FILE *stream)
{
    USART1->DR = c;//将c赋给串口1的DR寄存器，即重定向到串口，也可以是其他的接口
    while(!(USART1->SR & (1 << 7))){};//等待数据发送完成
    return c;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
