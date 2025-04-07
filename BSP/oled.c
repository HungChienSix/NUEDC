#include "oled.h"
#include "i2c.h"
#include "stdio.h" //添加头文件,可能需要改动
	
/*
	u8x8_byte_hw_i2c函数和u8x8_gpio_and_delay函数是移植函数,一般不需要更改
*/
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	/* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
  static uint8_t buffer[128];
  static uint8_t buf_idx;
  uint8_t *data;
  switch (msg)
  {
		case U8X8_MSG_BYTE_INIT:
  {
		/* add your custom code to init i2c subsystem */
    //MX_I2C2_Init(); //I2C初始化
  }
  break;
  case U8X8_MSG_BYTE_START_TRANSFER:
  {
		buf_idx = 0;
  }
  break;
  case U8X8_MSG_BYTE_SEND:
  {
		data = (uint8_t *)arg_ptr;
    while (arg_int > 0)
    {
			buffer[buf_idx++] = *data;
      data++;
      arg_int--;
    }
	}
  break;
  case U8X8_MSG_BYTE_END_TRANSFER:
  {
		if (HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, buffer, buf_idx, 1000) != HAL_OK)
    return 0;
  }
  break;
 
  case U8X8_MSG_BYTE_SET_DC:
  break;
 
  default:
		return 0;
	}
	return 1;
}
	
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
        __NOP();
        break;
    case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
        for (uint16_t n = 0; n < 320; n++)
        {
            __NOP();
        }
        break;
    case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
        HAL_Delay(1);
        break;
    case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        HAL_Delay(0);
        break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
        break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
        break;                    // arg_int=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
        u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_NEXT:
        u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_PREV:
        u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
        break;
    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}


/*
	OLED初始化函数
	U8g2的初始化，需要调用下面这个u8g2_Setup_ssd1306_128x64_noname_f函数，该函数的4个参数含义：
	u8g2：传入的U8g2结构体
	U8G2_R0：默认使用U8G2_R0即可（用于配置屏幕是否要旋转）
	u8x8_byte_sw_i2c：使用软件IIC驱动，该函数由U8g2源码提供
	u8x8_gpio_and_delay：就是上面我们写的配置函数
*/
void OLED_Init(u8g2_t *u8g2)
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8x8_gpio_and_delay);
	u8g2_InitDisplay(u8g2);                                                                       
	u8g2_SetPowerSave(u8g2, 0);                                                                   
	u8g2_ClearBuffer(u8g2);
}


/*
	输出变量
	使用如下函数,该函数是printf函数重定向
	int oled_printf(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *fmt,...)
*/
int OLED_Printf(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, const char *fmt,...)
{
	va_list ap;
	va_start(ap, fmt);
	int length;
	char buffer[128];
	length = vsnprintf(buffer, 128, fmt, ap);
	//u8g2_DrawStr(u8g2, x, y, buffer);
	u8g2_DrawUTF8(u8g2, x, y, buffer);
	va_end(ap);
	return length;
}	


/*
	输出图片
	使用如下函数
	void u8g2_DrawXBM(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h, const uint8_t *bitmap)
	void u8g2_DrawXBMP(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h, const uint8_t *bitmap)
	x 左上角坐标x
	y 左上角坐标y
	w 图形宽度
	h 图形高度
	bitmap 图形内容
	Img是一个图像示例
*/
const uint8_t Img[1024] =  {
0X00,0X00,0X00,0X00,0X00,0X00,0X1E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,
0X7F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFC,0XFF,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0XFF,0XFF,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0XFF,0XFF,0X03,
0X00,0X00,0X00,0X00,0X00,0X00,0XF0,0XFF,0XFF,0X07,0X00,0X00,0X00,0X00,0X00,0X00,
0XFC,0XFF,0XFF,0X07,0X00,0X00,0X00,0X00,0X00,0X00,0XFD,0XFF,0XFF,0X0F,0X00,0X00,
0X00,0X00,0XF0,0X83,0XFC,0XFF,0XFF,0X1F,0X00,0X00,0X00,0X00,0X04,0X68,0XFC,0XFF,
0XFF,0X1F,0X00,0X00,0X00,0X00,0X00,0X30,0XFC,0XFF,0XFF,0X3F,0X00,0X00,0X00,0X00,
0X28,0X12,0XF8,0XFF,0X7F,0X3F,0X00,0X00,0X00,0X00,0X7A,0X14,0XF8,0XFF,0XFF,0X38,
0X01,0X00,0X00,0X40,0X00,0X03,0XF0,0XFF,0XFF,0X01,0X00,0X00,0X00,0X00,0X84,0X0A,
0XE0,0XFF,0XFF,0X05,0X04,0X00,0X00,0X40,0X60,0X20,0XC0,0XFF,0XFF,0X01,0X00,0X00,
0X00,0X60,0X21,0X04,0X00,0XFF,0XFF,0X00,0X00,0X00,0X00,0X20,0X05,0X44,0X00,0XFC,
0X7F,0X04,0X00,0X00,0X00,0X20,0X89,0X80,0X00,0XE0,0X3F,0X00,0X00,0X00,0X00,0X00,
0X08,0X00,0X01,0X00,0X80,0X08,0X02,0X00,0X00,0X80,0X00,0X01,0X02,0X00,0X00,0XE0,
0X00,0X00,0X00,0XC0,0X05,0X02,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X40,0X00,
0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X10,0X00,0X80,0X00,0X80,0X00,0X00,0X00,
0X00,0X00,0X90,0X00,0X09,0X04,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XC4,
0XCF,0X00,0X00,0X00,0X00,0X00,0X24,0X00,0X21,0X2A,0X04,0X00,0X00,0X00,0X00,0XC0,
0X03,0X00,0X1F,0X91,0X3E,0X01,0X00,0X00,0X00,0XF0,0X07,0X00,0XFE,0XC0,0X17,0X01,
0X00,0X00,0X00,0XF8,0XF7,0X01,0XFE,0X80,0X17,0X01,0X00,0X00,0X00,0XF8,0XCF,0X00,
0X7E,0X80,0X17,0X01,0X00,0X00,0X00,0XF8,0X0F,0X01,0X7E,0X80,0X1F,0X01,0X00,0X00,
0X00,0XF0,0X0F,0X00,0X7E,0X80,0X27,0X01,0X00,0X00,0X00,0XE0,0X1F,0X00,0X7E,0X80,
0X07,0X01,0X00,0X00,0X00,0XE0,0X1F,0X01,0X3C,0X80,0X07,0X00,0X00,0X00,0X00,0XC0,
0X3F,0XC0,0X18,0X00,0X02,0X00,0X00,0X00,0X00,0X80,0X7F,0X82,0X00,0X40,0X20,0X00,
0X00,0X00,0X00,0X00,0XE7,0X83,0X00,0X40,0X10,0X00,0X00,0X00,0X00,0X00,0XE0,0X0B,
0X00,0X00,0X86,0X00,0X00,0X00,0X00,0X00,0XF0,0XCF,0X83,0XE0,0X03,0X00,0X00,0X00,
0X00,0X00,0XF0,0X8F,0XFF,0XFF,0X4B,0X00,0X00,0X00,0X00,0X00,0XF0,0XCF,0XFF,0X7F,
0X20,0X00,0X00,0X00,0X00,0X00,0XF0,0X9F,0XFF,0X3F,0X00,0X00,0X00,0X00,0X00,0X00,
0XE0,0X08,0XF8,0X9F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0X1F,0X01,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0X09,0X02,0X00,0X00,0X00,0X00,0X30,0X04,0XC0,
0XC1,0X10,0X01,0X00,0X00,0X00,0X00,0X02,0X20,0XC0,0XFF,0XFF,0X00,0X00,0X00,0X00,
0X80,0X00,0X80,0XF0,0XFF,0XE0,0X01,0X00,0X00,0X00,0X40,0X00,0X00,0XFC,0XFF,0XE1,
0X07,0X00,0X00,0X00,0X20,0X00,0X00,0XFC,0XFF,0XC1,0X1F,0X00,0X00,0X00,0X00,0X00,
0X00,0XFC,0XF0,0XC1,0X3F,0X00,0X00,0X00,0X00,0X00,0X80,0X08,0XC0,0X81,0XFF,0X00,
0X00,0X00,0X00,0X00,0X00,0X09,0X80,0X81,0XFF,0X01,0X00,0X00,0X08,0X00,0X00,0X00,
0X80,0X81,0XFF,0X00,0X00,0X00,0X08,0X00,0X00,0XC0,0X80,0X01,0X0F,0X00,0X00,0X00,
0X08,0X00,0X00,0X40,0X90,0X00,0X00,0X00,0X00,0X00,0X08,0X00,0X00,0X40,0X00,0X00,
0X00,0X10,0X00,0X00,0X00,0X00,0X00,0X38,0X06,0X01,0X00,0X10,0X00,0X00,0X00,0X00,
0X00,0X0A,0X30,0X00,0X00,0X03,0X00,0X00,0X00,0X00,0X00,0X00,0X20,0X00,0X0F,0X00,
0X00,0X00,0X20,0X00,0X00,0X00,0X20,0XF0,0X00,0X00,0X00,0X00,0X40,0X00,0X00,0X00,
0X00,0X60,0X00,0X00,0X00,0X00,0X80,0X00,0X00,0X00,0X08,0X00,0X00,0X00,0X00,0X00,
};

