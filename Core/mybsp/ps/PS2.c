/*********************************************************
*											PS2�ֱ�ͷ�ļ�
*										�ֱ��ӿڽ��߽��ܣ�
*										DI ->PA0 ��MOSI��
*										DO ->PA1 ��MISO��
*										CS ->PA2 ��CS��
*										CLK->PA3 ��SCLK��
**********************************************************/
#include "..\myapp\MyApplication.h"



#define DELAY_TIME  public.HAL_delay_us(5);
uint16_t Handkey;																							 //���ڴ洢����ֵ
uint8_t Comd[2]={0x01,0x42};																		 //0x01��ʼ���0x42�������ݣ����������ֲᣩ
uint8_t Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //�洢���յ�������
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
	};	//����ֵ�밴����

/*****************************************************
* @function     PS2�ֱ����ų�ʼ��
* @param        ��
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        PS2�ֱ���ʼ��
*****************************************************/
void PS2_Init(void)
{
	RCC->APB2ENR|=1<<2;     //ʹ��PORTAʱ��
	GPIOA->CRL&=0XFFFF000F; //PA1 2 3�������
	GPIOA->CRL|=0X00003330;

	GPIOA->CRL&=0XFFFFFFF0;
	GPIOA->CRL|=0X00000008; //PA0 ���ó�����	Ĭ������
}


/*****************************************************
* @function     ���ֱ�����ָ��
* @param        ָ��CMD
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        �޷���ֵ
*****************************************************/
void PS2_Cmd(uint8_t CMD)
{
    HAL_SPI_Transmit(&hspi2,&CMD,sizeof(CMD),5);
	public.HAL_delay_us(16);
}


/*****************************************************
* @function     �ֱ�ģʽ���ٺ��ģʽ ���̵�ģʽ
* @param        ָ��CMD
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        ����ֵ��0�����ģʽ����1���̵�ģʽ��
*								0x41=ģ���̵ƣ�0x73=ģ����
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
* @function     ��ȡ�ֱ�����
* @param        ��
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        �޷���ֵ
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
* @function     �����ȡ�������ֱ�����
* @param        ��
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        ֻ��һ����������ʱ����Ϊ0�� δ����Ϊ1
*****************************************************/
uint8_t PS2_DataKey()
{
	uint8_t index;

	PS2_ClearData();
	PS2_ReadData();

	Handkey=(Data[4]<<8)|Data[3];     //����16������  ����Ϊ0�� δ����Ϊ1
	for(index=0;index<16;index++)
	{
		if((Handkey&(1<<(MASK[index]-1)))==0)
			return index+1;
	}
	return 0;          //û���κΰ�������
}


/*****************************************************
* @function     ��ȡ�ֱ�ң��ģ��ֵ����Χ0~256
* @param        button
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief        ���ذ���ֵ
*****************************************************/
uint8_t PS2_AnologData(uint8_t button)
{
	return Data[button];
}


/*****************************************************
* @function     ������ݻ�����
* @param        ��
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
* @function     �ֱ���
* @param        motor1:�Ҳ�С�𶯵����0x00�أ�������
*								motor2:�����𶯵����0x40~0xFF���������ֵԽ����Խ��
* @file         PS2.c
* @author       hui
* @version      V0.1
* @date         2020.7.29
* @brief
*****************************************************/
void PS2_Vibration(uint8_t motor1, uint8_t motor2)
{
	public.HAL_delay_us(16);
    PS2_Cmd(0x01);  //��ʼ����
	PS2_Cmd(0x42); 		//��������
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
* @function     С������
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
* @function     PS2����
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
* @function     ����ģʽ����
*****************************************************/
void PS2_TurnOnAnalogMode(void)
{
    CS_L;
	PS2_Cmd(0x01);
	PS2_Cmd(0x44);
	PS2_Cmd(0X00);
	PS2_Cmd(0x01); 	//analog=0x01;digital=0x00  ������÷���ģʽ
	PS2_Cmd(0xEE);  //Ox03�������ã�������ͨ��������MODE������ģʽ��
									//0xEE������������ã���ͨ��������MODE������ģʽ��
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
    CS_H;
	public.HAL_delay_us(16);
}


/*****************************************************
* @function    ������
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
* @function    ��ɲ���������
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
* @function    �ֱ����ó�ʼ��
*****************************************************/
void PS2_SetInit(void)
{
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();			//��������ģʽ
	PS2_TurnOnAnalogMode();	//�����̵ơ�����ģʽ����ѡ���Ƿ񱣴�
	PS2_VibrationMode();	//������ģʽ
	PS2_ExitConfing();		  //��ɲ���������
}

















