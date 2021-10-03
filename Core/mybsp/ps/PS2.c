/*********************************************************
*											PS2手柄头文件
*										手柄接口接线介绍：
*										DI ->PA0 （MOSI）
*										DO ->PA1 （MISO）
*										CS ->PA2 （CS）
*										CLK->PA3 （SCLK）
**********************************************************/
#include "..\myapp\MyApplication.h"



#define DELAY_TIME  public.HAL_delay_us(5);
uint16_t Handkey;																							 //用于存储按键值
uint8_t Comd[2]={0x01,0x42};																		 //0x01开始命令，0x42请求数据（根据数据手册）
uint8_t Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //存储接收到的数据
uint16_t MASK[]={
    PSB_SELECT,
    PSB_L3,
    PSB_R3 ,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1 ,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
	};	//按键值与按键明

/*****************************************************
* @function     PS2手柄引脚初始化
* @param        无
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        PS2手柄初始化
*****************************************************/
void PS2_Init(void)
{
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	GPIOA->CRL&=0XFFFF000F; //PA1 2 3推挽输出
	GPIOA->CRL|=0X00003330;

	GPIOA->CRL&=0XFFFFFFF0;
	GPIOA->CRL|=0X00000008; //PA0 设置成输入	默认下拉
}


/*****************************************************
* @function     向手柄发送指令
* @param        指令CMD
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        无返回值
*****************************************************/
void PS2_Cmd(uint8_t CMD)
{
    HAL_SPI_Transmit(&hspi2,&CMD,sizeof(CMD),5);
	public.HAL_delay_us(16);
}


/*****************************************************
* @function     手柄模式：①红灯模式 ②绿灯模式
* @param        指令CMD
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        返回值：0（红灯模式），1（绿灯模式）
*								0x41=模拟绿灯，0x73=模拟红灯
*****************************************************/
uint8_t PS2_RedLight(void)
{
    HAL_SPI_TransmitReceive(&hspi2,&Comd[0],&Data[0],1,10);
    HAL_Delay(1);
    HAL_SPI_TransmitReceive(&hspi2,&Comd[1],&Data[1],1,10);
    if( Data[1] == 0X73)
        return 0 ;
    else
        return 1;
}


/*****************************************************
* @function     读取手柄数据
* @param        无
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        无返回值
*****************************************************/
void PS2_ReadData(void)
{
    CS_L;
    HAL_SPI_TransmitReceive(&hspi2,&Comd[0],&Data[0],1,10);
    HAL_Delay(1);
    HAL_SPI_TransmitReceive(&hspi2,&Comd[1],&Data[1],1,10);
    HAL_Delay(1);
//    HAL_SPI_TransmitReceive(&hspi2,&Comd[2],&Data[2],7,100);
    for(int i=2;i<9;i++) {
        HAL_SPI_Receive_DMA(&hspi2, &Data[i], 1);
    HAL_Delay(1);
    }

    CS_H;
}


/*****************************************************
* @function     处理读取出来的手柄数据
* @param        无
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        只有一个按键按下时按下为0， 未按下为1
*****************************************************/
uint8_t PS2_DataKey()
{
	uint8_t index;

	PS2_ClearData();
	PS2_ReadData();

	Handkey=(Data[4]<<8)|Data[3];     //这是16个按键  按下为0， 未按下为1
	for(index=0;index<16;index++)
	{
		if((Handkey&(1<<(MASK[index]-1)))==0)
			return index+1;
	}
	return 0;          //没有任何按键按下
}


/*****************************************************
* @function     获取手柄遥感模拟值：范围0~256
* @param        button
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        返回按键值
*****************************************************/
uint8_t PS2_AnologData(uint8_t button)
{
	return Data[button];
}


/*****************************************************
* @function     清除数据缓冲区
* @param        无
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief
*****************************************************/
void PS2_ClearData()
{
	uint8_t index;
	for(index=0;index<9;index++)
		Data[index]=0x00;
}


/*****************************************************
* @function     手柄震动
* @param        motor1:右侧小震动电机，0x00关，其他开
*								motor2:左侧大震动电机，0x40~0xFF，电机开，值越大，震动越大
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief
*****************************************************/
void PS2_Vibration(uint8_t motor1, uint8_t motor2)
{
	public.HAL_delay_us(16);
    PS2_Cmd(0x01);  //开始命令
	PS2_Cmd(0x42); 		//请求数据
	PS2_Cmd(0X00);
	PS2_Cmd(motor1);
	PS2_Cmd(motor2);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);

	public.HAL_delay_us(16);
}


/*****************************************************
* @function     小幅度震动
*****************************************************/
void PS2_ShortPoll(void)
{
    CS_L;
	public.HAL_delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
    CS_H;
	public.HAL_delay_us(16);
}


/*****************************************************
* @function     PS2配置
*****************************************************/
void PS2_EnterConfing(void)
{
    CS_L;
	public.HAL_delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0X00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
    CS_H;
	public.HAL_delay_us(16);
}


/*****************************************************
* @function     发送模式设置
*****************************************************/
void PS2_TurnOnAnalogMode(void)
{
    CS_L;
	PS2_Cmd(0x01);
	PS2_Cmd(0x44);
	PS2_Cmd(0X00);
	PS2_Cmd(0x01); 	//analog=0x01;digital=0x00  软件设置发送模式
	PS2_Cmd(0xEE);  //Ox03锁存设置，即不可通过按键“MODE”设置模式。
									//0xEE不锁存软件设置，可通过按键“MODE”设置模式。
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
    CS_H;
	public.HAL_delay_us(16);
}


/*****************************************************
* @function    振动设置
*****************************************************/
void PS2_VibrationMode(void)
{
	;
	public.HAL_delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x4D);
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0X01);

	public.HAL_delay_us(16);
}


/*****************************************************
* @function    完成并保存配置
*****************************************************/
void PS2_ExitConfing(void)
{
    CS_L;
	public.HAL_delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
    CS_H;
	public.HAL_delay_us(16);
}


/*****************************************************
* @function    手柄配置初始化
*****************************************************/
void PS2_SetInit(void)
{
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();			//进入配置模式
	PS2_TurnOnAnalogMode();	//“红绿灯”配置模式，并选择是否保存
	PS2_VibrationMode();	//开启震动模式
	PS2_ExitConfing();		  //完成并保存配置
}

















