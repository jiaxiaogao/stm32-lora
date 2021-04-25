#include "esp8266.h"
uint8_t CMDBUF[CMDBUFSIZE];
uint8_t DATABUF[CMDBUFSIZE];

uint8_t send_cmd(char *cmd, char * ret)
{
	// printf("SR: -%#lX-\r\n", huart2.Instance->SR);
	// printf("CR: -%#lX-\r\n", huart2.Instance->CR1);
	memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
	USART2_RX_STA = 0;
	memset(CMDBUF, 0, sizeof(CMDBUF));
	strcpy((char * restrict)CMDBUF, cmd);
	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
	while((USART2_RX_STA & 0x8000) == 0)printf("11\r\n");
	printf("%s---%s-\r\n", cmd, USART2_RX_BUF);

	if(strstr((const char *)USART2_RX_BUF, (const char *)ret) != NULL)//包含ret子串
		return 1;
	else
		return 0;
}


/*
uint8_t esp8266_init()
{
	//刚开始上电要延时5s，模块会根据上一次的数据进行链接检测
	HAL_Delay(5000);
	//检测是否存在
	while(send_cmd("AT\r\n", "OK") == 0)
	{
		printf("正在检测ESP8266设备...\r\n");
		HAL_Delay(2000);
	}
	HAL_Delay(2000);

	// //关闭回显
	// while(send_cmd("ATE0\r\n", "OK") == 0)
	// {
	// 	printf("正在关闭回显...\r\n");
	// 	HAL_Delay(100);
	// }
	//设为STA模式 
	while(send_cmd("AT+CWMODE=1\r\n", "OK") == 0)
	{
		printf("正在设为STA模式...\r\n");
		HAL_Delay(2000);
	}
	HAL_Delay(2000);

	// 重启
	// while(send_cmd("AT+RST\r\n", "OK") == 0)
	// {
	// 	printf("正在重启...\r\n");
	// 	HAL_Delay(10000);
	// }
	HAL_Delay(10000);
	
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_IDLE);
	// HAL_Delay(10000);//问题不在延时这里，那就应该在串口那了，串口接受的数据太乱了？？？
	// printf("000\r\n");
	// HAL_Delay(1000);
	// 	printf("001\r\n");
	// HAL_Delay(1000);
	// 	printf("002\r\n");
	// HAL_Delay(1000);
	// 	printf("003\r\n");
	// HAL_Delay(1000);
	// 	printf("004\r\n");
	// HAL_Delay(1000);
	// 	printf("005\r\n");
	// HAL_Delay(1000);
	// 	printf("006\r\n");
	// HAL_Delay(1000);
	// 	printf("007\r\n");
	// HAL_Delay(1000);
	// printf("008\r\n");
	// HAL_Delay(1000);
	// 	printf("009\r\n");
	// HAL_Delay(1000);
	// printf("010\r\n");
	// __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	// 	printf("011\r\n");
	// __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);

	//连接路由器：命令执行不会立刻返回OK，配置成功之后才会有OK，这个OK等不来，不等了，这一句不是OK，下面AT+CIFSR也不会是OK
	// send_cmd("AT+CWJAP=\"AlbusInk\",\"AlbusInk\"\r\n", "OK");
	// // send_cmd("AT+CWJAP=\"SWUN-EDU6\",\"4265jqjxggxrrll\"\r\n", "OK");
	// printf("000\r\n");
	// HAL_Delay(10000);
	// printf("000\r\n");

	//查询IP，查询不成功说明连接失败，再来一次
	// while(send_cmd("AT+CIFSR\r\n", "OK") == 0)
	// {
	// 	printf("正在连接路由器...\r\n");
	// 	send_cmd("AT+CWJAP=\"AlbusInk\",\"AlbusInk\"\r\n", "OK");
	// 	// send_cmd("AT+CWJAP=\"SWUN-EDU6\",\"4265jqjxggxrrll\"\r\n", "OK");
	// 	HAL_Delay(10000);
	// 	printf("正在查询IP...\r\n");
	// 	HAL_Delay(2000);
	// }
	// HAL_Delay(2000);

	printf("000\r\n");
	//模块连接到server：命令执行立刻返回OK，也有可能不会，不等了，这一句不是OK，下面AT+CIPSEND也不会是OK
	// send_cmd("AT+CIPSTART=\"TCP\",\"192.168.43.167\",12345\r\n", "OK");
	// send_cmd("AT+CIPSTART=\"TCP\",\"212.64.11.133\",80\r\n", "OK");
	// send_cmd("AT+CIPSTART=\"TCP\",\"www.baidu.com\",80\r\n", "OK");
	HAL_UART_Transmit(&huart2, "AT+CIPSTART=\"TCP\",\"212.64.11.133\",80\r\n" , strlen((const char *)"AT+CIPSTART=\"TCP\",\"212.64.11.133\",80\r\n"), 1000);
	HAL_Delay(3000);
	printf("000\r\n");

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	//开启透传模式
	while(send_cmd("AT+CIPMODE=1\r\n", "OK") == 0)
	{
		printf("正在开启透传模式...\r\n");
		HAL_Delay(1000);
	}
	HAL_Delay(1000);

	//开始透传，开启失败说明没有连接成功，再来一次，且透传也要再重新打开
	while(send_cmd("AT+CIPSEND\r\n", "OK") == 0)
	{
		printf("模块正在连接到server...\r\n");
		// send_cmd("AT+CIPSTART=\"TCP\",\"192.168.43.86\",12345\r\n", "OK");
		send_cmd("AT+CIPSTART=\"TCP\",\"212.64.11.133\",80\r\n", "OK");
		HAL_Delay(3000);
		//开启透传模式
		while(send_cmd("AT+CIPMODE=1\r\n", "OK") == 0)
		{
			printf("正在开启透传模式...\r\n");
			HAL_Delay(1000);
		}
		HAL_Delay(1000);
		printf("正在开始透传...\r\n");
		HAL_Delay(1000);
	}
	//http
	send_cmd("GET http://212.64.11.133/index.jsp?wendu=20&shidu=90\r\n", "<html>");
	HAL_Delay(1000);
	return 1;
}
*/



//-----------------------------------------------------------------
uint8_t esp8266_init_test()
{
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_IDLE);

	//刚开始上电要延时5s，模块会根据上一次的数据进行链接检测
	HAL_Delay(5000);
	//检测是否存在
	memset(CMDBUF, 0, sizeof(CMDBUF));
	strcpy((char * restrict)CMDBUF, "AT\r\n");
	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
	HAL_Delay(2000);

	//设为STA模式 
	memset(CMDBUF, 0, sizeof(CMDBUF));
	strcpy((char * restrict)CMDBUF, "AT+CWMODE=1\r\n");
	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
	HAL_Delay(2000);
	
	// 重启
	// while(send_cmd("AT+RST\r\n", "OK") == 0)
	// {
	// 	printf("正在重启...\r\n");
	// 	HAL_Delay(10000);
	// }
	HAL_Delay(10000);
	
	
	// send_cmd("AT+CWJAP=\"AlbusInk\",\"AlbusInk\"\r\n", "OK");
	// // send_cmd("AT+CWJAP=\"SWUN-EDU6\",\"4265jqjxggxrrll\"\r\n", "OK");
	// printf("000\r\n");
	// HAL_Delay(10000);
	// printf("000\r\n");

	//查询IP，查询不成功说明连接失败，再来一次
	// while(send_cmd("AT+CIFSR\r\n", "OK") == 0)
	// {
	// 	printf("正在连接路由器...\r\n");
	// 	send_cmd("AT+CWJAP=\"AlbusInk\",\"AlbusInk\"\r\n", "OK");
	// 	// send_cmd("AT+CWJAP=\"SWUN-EDU6\",\"4265jqjxggxrrll\"\r\n", "OK");
	// 	HAL_Delay(10000);
	// 	printf("正在查询IP...\r\n");
	// 	HAL_Delay(2000);
	// }
	// HAL_Delay(2000);

	printf("000\r\n");
	//模块连接到server：命令执行立刻返回OK，也有可能不会，不等了，这一句不是OK，下面AT+CIPSEND也不会是OK
	memset(CMDBUF, 0, sizeof(CMDBUF));
	strcpy((char * restrict)CMDBUF, "AT+CIPSTART=\"TCP\",\"212.64.11.133\",80\r\n");
	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
	HAL_Delay(3000);
	printf("000\r\n");


	//开启透传模式
	memset(CMDBUF, 0, sizeof(CMDBUF));
	strcpy((char * restrict)CMDBUF, "AT+CIPMODE=1\r\n");
	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
	HAL_Delay(1000);

	//开始透传，开启失败说明没有连接成功，再来一次，且透传也要再重新打开

	memset(CMDBUF, 0, sizeof(CMDBUF));
	strcpy((char * restrict)CMDBUF, "AT+CIPSEND\r\n");
	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
	HAL_Delay(1000);
	
	//http
	memset(CMDBUF, 0, sizeof(CMDBUF));
	strcpy((char * restrict)CMDBUF, "GET http://212.64.11.133/index.jsp?wendu=11&shidu=111\r\n");
	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
	HAL_Delay(1000);
	
	
	// __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	// __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	return 1;
}



void send_data(uint8_t * data)
{
	HAL_UART_Transmit(&huart2, data , strlen((const char *)data), 1000);
//	bzero(data, 0);
    memset(data, 0, 1000);
}


void send_data_test(uint8_t wendu, uint8_t shidu)
{
	memset(CMDBUF, 0, sizeof(CMDBUF));
	sprintf((char * restrict)CMDBUF, "GET http://212.64.11.133/index.jsp?wendu=%d&shidu=%d\r\n", wendu, shidu);
	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
	// HAL_Delay(10);
}