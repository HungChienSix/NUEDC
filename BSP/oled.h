#ifndef OLED_H
#define OLED_H
 
#include "main.h"
#include "u8g2.h"//可能需要改头文件包含,

#define MAX_LEN    128  
#define OLED_ADDRESS  0x78 
#define OLED_CMD   0x00 
#define OLED_DATA  0x40  
 
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void OLED_Init(u8g2_t *u8g2);
int OLED_Printf(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *fmt,...);
 
#endif