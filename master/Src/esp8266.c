#include "esp8266.h"
#include "main.h"
uint8_t CMDBUF[CMDBUFSIZE];//只是用来中转了，也是一个中间变量，全部在这个文件中用的
uint8_t CMDBUF_2[CMDBUFSIZE];
uint8_t DATABUF[CMDBUFSIZE];//只是在主函数当作一个中间变量来用了，在这边没有用到

//发送命令，并使用数据帧中断的方式接收串口收到的数据并判断（下面的init基本上没有用这个）
uint8_t send_cmd(char *cmd, char * ret)
{
//    uint32_t temp = 0;
  
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);//打开数据帧接收模式，进来打开，出去就关了
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
   
//    temp = huart2.Instance->CR1;
//    
//    printf("--%#lx--\r\n", temp);
  
  
  memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
  USART2_RX_STA = 0;
  memset(CMDBUF, 0, sizeof(CMDBUF));
  strcpy((char * )CMDBUF, cmd);
  
//  printf("+++%s+++\r\n", CMDBUF);
  
  HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
  
  led_debug();//debug：红灯
  //  HAL_Delay(100);
  while((USART2_RX_STA & 0x8000) == 0)HAL_Delay(1);//      printf("11\r\n");
  //  printf("-%#lX-\r\n", USART2_RX_STA);
  printf("%s---%s-\r\n", cmd, USART2_RX_BUF);
  
  __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);//关闭数据帧接收模式，下面就不需要回复数据了
  __HAL_UART_DISABLE_IT(&huart2, UART_IT_IDLE);
  
  if(strstr((const char *)USART2_RX_BUF, (const char *)ret) != NULL)//包含ret子串
    return 1;
  else
    return 0;
}




//初始化，这个初始化函数并没有判断esp返回的响应数据，只是简单的配置
uint8_t esp8266_init_test()//统一不判断回复，发送就完了，不成功复位重来
{
  //  if(send_cmd("AT+CIPSEND\r\n", "OK") == 1)
  //  {
  //    led_debug();//debug：红灯
  //  }
  //  while(1);
  //  //测试接收回显
  
  
  //关闭了字节中断和帧中断，因为下面的初始化方式不需要判断回复的数据
  __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
  __HAL_UART_DISABLE_IT(&huart2, UART_IT_IDLE);
  
  //刚开始上电要延时5s，模块会根据上一次的数据进行链接检测
  
  HAL_Delay(5000);
  //可以不用重新连接，esp有记忆功能，可以记下上一次的连接，等待5s让他连接就行了
  
//  
//  	//1、检测是否存在
//  	memset(CMDBUF, 0, sizeof(CMDBUF));
//  	strcpy((char *)CMDBUF, "AT\r\n");
//  	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//  	HAL_Delay(2000);
//      led_debug();//debug：红灯
//      
//  
//  	//2、设为STA模式 
//  	memset(CMDBUF, 0, sizeof(CMDBUF));
//  	strcpy((char *)CMDBUF, "AT+CWMODE=1\r\n");
//  	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//  	HAL_Delay(2000);
//      led_init();//LED初始化为三色
//  	
//  	 //3、重启
//      memset(CMDBUF, 0, sizeof(CMDBUF));
//  	strcpy((char *)CMDBUF, "AT+RST\r\n");
//  	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//  	HAL_Delay(10000);
//      led_debug();//debug：红灯
//  	
//  	//4、连接WiFi
//      memset(CMDBUF, 0, sizeof(CMDBUF));
//  	strcpy((char *)CMDBUF, "AT+CWJAP=\"jiaxiaogao\",\"jiaxiaogao\"\r\n");
//  	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//  	HAL_Delay(10000);
//      led_init();//LED初始化为三色
  
  
  //到后面就不用上面的了，因为有记录的！！
  
  //下面的这些肯定回回复OK的就用send_cmd函数来发送，以判断一下回复的OK（暂时还没有做）
  //目前使用正常了，暂时不做这个判断了，有点小麻烦，到时候可能sendcmd函数还会出问题啥的，先不判断了
  
  
  //5、查询IP 命令执行立刻返回OK
  led_debug();//debug：红灯
  //  send_cmd("AT+CIFSR\r\n", "OK");
  memset(CMDBUF, 0, sizeof(CMDBUF));
  strcpy((char *)CMDBUF, "AT+CIFSR\r\n");
  HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
  HAL_Delay(2000);
  
  
  //LOOP:  
  
  //6、模块连接到server，命令执行立刻返回OK，也有可能不会，不等了，这一句不是OK，下面AT+CIPSEND也不会是OK
  led_init();//LED初始化为三色
  //  send_cmd("AT+CIPSTART=\"TCP\",\"116.63.148.112\",80\r\n", "OK");
  memset(CMDBUF, 0, sizeof(CMDBUF));
  //  //	strcpy((char * restrict)CMDBUF, "AT+CIPSTART=\"TCP\",\"192.168.0.109\",8080\r\n");
  strcpy((char *)CMDBUF, "AT+CIPSTART=\"TCP\",\"47.108.150.205\",8848\r\n");
  HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
  HAL_Delay(3000);
  
  
  
  //7、开启透传模式。命令执行立刻返回OK
  led_debug();//debug：红灯
  //  send_cmd("AT+CIPMODE=1\r\n", "OK");
  memset(CMDBUF, 0, sizeof(CMDBUF));
  strcpy((char *)CMDBUF, "AT+CIPMODE=1\r\n");
  HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
  HAL_Delay(1000);
  
  
  //8、开始透传，命令执行立刻返回OK
  
  led_init();//LED初始化为三色
  //  printf("1\r\n");
  //  send_cmd("AT+CIPSEND\r\n", "OK");
  memset(CMDBUF, 0, sizeof(CMDBUF));
  strcpy((char * )CMDBUF, "AT+CIPSEND\r\n");
  HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
  HAL_Delay(1000);
  led_debug();//debug：红灯
  
  
  //8、开始透传，命令执行立刻返回OK
  
  //	memset(CMDBUF, 0, sizeof(CMDBUF));
  //	strcpy((char * restrict)CMDBUF, "AT+CIPSEND\r\n");
  //    led_rev();//接收时：绿灯
  //	if(send_cmd(CMDBUF, "OK"))
  //    {   
  //      led_debug2();//debug：红+绿灯
  //      HAL_Delay(1000);
  //      
  //    }
  //    else//如果开启失败，重启一下，从IP处开始重新配置
  //    {
  //      //3、重启
  //      
  //      led_init();//LED初始化为三色
  //      memset(CMDBUF, 0, sizeof(CMDBUF));
  //      strcpy((char * restrict)CMDBUF, "AT+RST\r\n");
  //      HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
  //      HAL_Delay(10000);
  //
  //      goto LOOP;
  //    }
  
  
  
  //发一条试一下透传
  memset(CMDBUF, 0, sizeof(CMDBUF));
  sprintf((char * )CMDBUF, "---test---\r\n");
  HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
  HAL_Delay(1000);
  led_debug();//debug：红灯
  
  
  //////
  //////	//http
  //////	memset(CMDBUF, 0, sizeof(CMDBUF));
  //////	strcpy((char * restrict)CMDBUF, "GET http://212.64.11.133/index.jsp?wendu=11&shidu=111\r\n");
  //////	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
  //////	HAL_Delay(1000);
  
  
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


void send_data_test(uint8_t request_num, uint8_t wendu, uint8_t shidu)
{
//  uint8_t guangzhao = 0;
//  uint8_t co2 = 0;  
//  uint8_t device_ok = 1;
  memset(CMDBUF_2, 0, sizeof(CMDBUF_2));
//  memset(CMDBUF, 0, sizeof(CMDBUF));
  //////	sprintf((char * restrict)CMDBUF, "GET http://212.64.11.133/index.jsp?wendu=%d&shidu=%d\r\n", wendu, shidu);
  //////	sprintf((char * restrict)CMDBUF, "%d:wendu=%d&shidu=%d\r\n",request_num, wendu, shidu);
  sprintf((char *)CMDBUF_2, 
//          "GET http://116.63.148.112:80/SmartTea_v1/controller/HandleDevice.php?push_data=push_data&&wendu=%d&&shidu=%d&&guangzhao=%d&&co2=%d\r\n", 
//          "GET http://116.63.148.112:80/smart_tea/controller/HandleDevice.php?push_data=push_data&&device_id=%d&&device_ok=%d&&wendu=%d&&shidu=%d&&guangzhao=%d&&co2=%d\r\n",
//          request_num, device_ok, wendu, shidu, guangzhao, co2);
          "GET http://47.108.150.205:8848/get_data/?device_id=%d&&wendu=%d&&shidu=%d\r\n",request_num, wendu, shidu);
//  HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//  printf("--%s--\r\n", CMDBUF_2);
  
  HAL_UART_Transmit(&huart2, CMDBUF_2 , strlen((const char *)CMDBUF_2), 1000);
//  send_cmd(CMDBUF_2, "OK");
  //注：如果这里用CMDBUF就不行，因为CMDBUF是uint8_t类型的！！

}





//目前的问题：需要把串口2哪里好好再整理一下，要求：不知能发送，也要能接收，要回显的。
//看以前的博客，有关于串口接收方式的内容！！
//没有用串口接收！直接发的！
//注意：
//每次测试的时候要给esp8266断电重连，不然会一直保持透传状态！





/*之前的，可能用不着了，先放这吧
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
//uint8_t esp8266_init_test()
//{
//	__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
//	__HAL_UART_DISABLE_IT(&huart2, UART_IT_IDLE);
//
//	//刚开始上电要延时5s，模块会根据上一次的数据进行链接检测
//	HAL_Delay(5000);
//	//检测是否存在
//	memset(CMDBUF, 0, sizeof(CMDBUF));
//	strcpy((char * restrict)CMDBUF, "AT\r\n");
//	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//	HAL_Delay(2000);
//
//	//设为STA模式 
//	memset(CMDBUF, 0, sizeof(CMDBUF));
//	strcpy((char * restrict)CMDBUF, "AT+CWMODE=1\r\n");
//	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//	HAL_Delay(2000);
//	
//	 //重启
//	 while(send_cmd("AT+RST\r\n", "OK") == 0)
//	 {
//	 	printf("正在重启...\r\n");
//	 	HAL_Delay(10000);
//	 }
//	HAL_Delay(10000);
//	
//	
//	 send_cmd("AT+CWJAP=\"xiaogaoder\",\"xiaogaoder\"\r\n", "OK");
//	 // send_cmd("AT+CWJAP=\"SWUN-EDU6\",\"4265jqjxggxrrll\"\r\n", "OK");
//	 printf("000\r\n");
//	 HAL_Delay(10000);
//	 printf("000\r\n");
//
////	查询IP，查询不成功说明连接失败，再来一次
////	 while(send_cmd("AT+CIFSR\r\n", "OK") == 0)
////	 {
////	 	printf("正在连接路由器...\r\n");
////	 	send_cmd("AT+CWJAP=\"AlbusInk\",\"AlbusInk\"\r\n", "OK");
////	 	// send_cmd("AT+CWJAP=\"SWUN-EDU6\",\"4265jqjxggxrrll\"\r\n", "OK");
////	 	HAL_Delay(10000);
////	 	printf("正在查询IP...\r\n");
////	 	HAL_Delay(2000);
////	 }
//	 HAL_Delay(2000);
//
//	printf("000\r\n");
//	//模块连接到server：命令执行立刻返回OK，也有可能不会，不等了，这一句不是OK，下面AT+CIPSEND也不会是OK
//	memset(CMDBUF, 0, sizeof(CMDBUF));
//	strcpy((char * restrict)CMDBUF, "AT+CIPSTART=\"TCP\",\"192.168.0.108\",8080\r\n");
//	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//	HAL_Delay(3000);
//	printf("000\r\n");
//
//
//	//开启透传模式
//	memset(CMDBUF, 0, sizeof(CMDBUF));
//	strcpy((char * restrict)CMDBUF, "AT+CIPMODE=1\r\n");
//	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//	HAL_Delay(1000);
//
//	//开始透传，开启失败说明没有连接成功，再来一次，且透传也要再重新打开
//
//	memset(CMDBUF, 0, sizeof(CMDBUF));
//	strcpy((char * restrict)CMDBUF, "AT+CIPSEND\r\n");
//	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//	HAL_Delay(1000);
//	
//	//http
//	memset(CMDBUF, 0, sizeof(CMDBUF));
//	strcpy((char * restrict)CMDBUF, "GET http://212.64.11.133/index.jsp?wendu=11&shidu=111\r\n");
//	HAL_UART_Transmit(&huart2, CMDBUF , strlen((const char *)CMDBUF), 1000);
//	HAL_Delay(1000);
//	
//	
//	// __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
//	// __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
//	return 1;
//}