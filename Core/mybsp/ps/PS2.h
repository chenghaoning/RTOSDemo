#ifndef __PS2_H
#define __PS2_H
#include "main.h"


/*********************************************************
*										手柄接口接线介绍：
*										DI ->PA0 （MOSI）
*										DO ->PA1 （MISO）
*										CS ->PA2 （CS）
*										CLK->PA3 （SCLK）
**********************************************************/

//#define DI   HAL_GPIO_ReadPin(DI_GPIO_Port,DI_Pin)          //PA0  输入
//
//#define DO_H HAL_GPIO_WritePin(DO_GPIO_Port,DO_Pin,SET)       //命令位高
//#define DO_L HAL_GPIO_WritePin(DO_GPIO_Port,DO_Pin,RESET)       //命令位低
//

#define NSS 0
#if NSS
#define CS_L UNUSED(1)
#define CS_H UNUSED(1)
#else
#define CS_H  HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,SET)     //CS拉高
#define CS_L  HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,RESET)       //CS拉低
#endif
//#define CLK_H HAL_GPIO_WritePin(CLK_GPIO_Port,CLK_Pin,SET)      //时钟拉高
//#define CLK_L HAL_GPIO_WritePin(CLK_GPIO_Port,CLK_Pin,RESET)      //时钟拉低


//These are our button constants
#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16

#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      16

//#define WHAMMY_BAR		8

//These are stick values
#define PSS_RX 5               //右摇杆X轴数据
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8

extern uint8_t Data[9];
extern uint16_t MASK[16];
extern uint16_t Handkey;

void PS2_Init(void);
uint8_t PS2_RedLight(void);
void PS2_ReadData(void);
void PS2_Cmd(uint8_t CMD);
uint8_t PS2_DataKey(void);
uint8_t PS2_AnologData(uint8_t button);
void PS2_ClearData(void);
void PS2_Vibration(uint8_t motor1, uint8_t motor2);

void PS2_EnterConfing(void);
void PS2_TurnOnAnalogMode(void);
void PS2_VibrationMode(void);
void PS2_ExitConfing(void);
void PS2_SetInit(void);

#endif

