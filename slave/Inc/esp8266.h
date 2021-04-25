#ifndef __ESP8266_H_
#define __ESP8266_H_

#include  "main.h"
#define CMDBUFSIZE  512
extern uint8_t CMDBUF[CMDBUFSIZE];
extern uint8_t DATABUF[CMDBUFSIZE];

uint8_t esp8266_init();
uint8_t esp8266_init_test();
void send_data(uint8_t * data);
void send_data_test(uint8_t wendu, uint8_t shidu);





#endif