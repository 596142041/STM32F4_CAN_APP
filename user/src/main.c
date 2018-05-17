/**
  ******************************************************************************
  * @file    main.c
  * $Author: 飞鸿踏雪 $
  * $Revision: 17 $
  ******************************************************************************/
#include "delay.h"
#include "can_app.h"  
#include "mb.h"  //FreeModbus
#include "mbport.h"
#include "port.h"
#include "stdio.h"
#include "USARt1.h"  
#include "timer.h"
#include "os.h"
#include "CANB.h"
extern CanRxMsg CAN1_RxMessage;
extern volatile uint8_t CAN1_CanRxMsgFlag;//接收到CAN数据后的标志 
void Led_GPIO_Config(void);
u16 count = 0;
void task_10ms(void);
void task_50ms(void);
void task_5ms(void);
void task_1000ms(void);
eMBErrorCode Code, error_code; 
int main(void)
{ 
	int i;
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);               //配置中断向量偏移
	delay_init(168);
	if(*((uint32_t*)APP_EXE_FLAG_ADDR)==0xFFFFFFFF)
	{
		static unsigned char data[4]={0x12,0x34,0x56,0x78};
		FLASH_Unlock();
		CAN_BOOT_ProgramDatatoFlash(APP_EXE_FLAG_ADDR,data,4);
		FLASH_Lock();
	}
	__enable_irq(); 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
	CAN_tx_msg_init();
	CAN_Configuration(250000);
	Led_GPIO_Config();
	taskinit();
	USART1_struct.USART_init(9600);
	TIM6_init(); 
	GPIO_SetBits(GPIOF,GPIO_Pin_7);
	GPIO_ResetBits(GPIOF,GPIO_Pin_8);
	while (1)
	{ 
	for(i = 0;i<TASKNUM;i++)
		{
			if(tasktimer[i].flag != 0)
			{
				switch (i)
				{
					case TASK10MS:
					{
						task_10ms();
					}
					break;
					case TASK50MS:
					{
						task_50ms();
					}
					break;
					case TASK5MS:
					{
						task_5ms();
					}
					break;
					case TASK1000MS:
					{
						  printf("TASK1000ms \n\r"); 
						task_1000ms();
					}
					break;
				}
			}
			tasktimer[i].flag = 0;
		}
	}
}
void task_5ms(void)
{
	
	if(CAN_MessagePending(CAN1,CAN_FIFO0) !=0)
	{
		CAN_Receive(CAN1,CAN_FIFO0, &CAN1_RxMessage);
		CAN1_CanRxMsgFlag = 1;
	}
	else
	{
		canbtransmit();
	}
	if(CAN1_CanRxMsgFlag)
	{
		CAN_BOOT_ExecutiveCommand(&CAN1_RxMessage);
		CAN1_CanRxMsgFlag = 0; 
	}
	 
}
void task_10ms(void)
{
	usRegHoldingBuf[0] = speed_ref;
	usRegHoldingBuf[1] = speed_fbk;
	usRegHoldingBuf[2] = dir;
	Code = eMBPoll();
}
void task_50ms(void)
{

}
void task_1000ms(void)
{ 
		if(GPIO_ReadOutputDataBit(GPIOF,GPIO_Pin_7) == SET)
		{
			GPIO_ResetBits(GPIOF,GPIO_Pin_7);
			
		}
		else
		{
			GPIO_SetBits(GPIOF,GPIO_Pin_7);
		}
		if(GPIO_ReadOutputDataBit(GPIOF,GPIO_Pin_8) == SET)
		{  
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
		}
		else
		{
			GPIO_SetBits(GPIOF,GPIO_Pin_8); 
		}
}
int fputc(int ch, FILE *f)
{
	USART1_struct.send_data(ch);
	return ch;
}
void Led_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_init;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	GPIO_init.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_7;
	GPIO_init.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_init.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_init.GPIO_OType = GPIO_OType_PP;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF,&GPIO_init);
	GPIO_SetBits(GPIOF,GPIO_Pin_7|GPIO_Pin_8); 
}

