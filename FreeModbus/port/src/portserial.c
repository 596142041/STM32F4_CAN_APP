#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#define USE_USART  USART3
#define USE_USART_CLK     RCC_APB1Periph_USART3
#define USART_Tx_GPIO     GPIOB
#define USART_Tx_Pin      GPIO_Pin_10
#define USART_Tx_GPIO_CLK RCC_AHB1Periph_GPIOB
#define USART_Rx_GPIO     GPIOB
#define USART_Rx_Pin      GPIO_Pin_11
#define USART_Rx_GPIO_CLK RCC_AHB1Periph_GPIOB
//---------------------------------------
#define USE_RS485					1
#define RS485_EN_GPIO     GPIOB
#define RS485_EN_Pin      GPIO_Pin_2
#define RS485_EN_GPIO_CLK    RCC_AHB1Periph_GPIOB
#define TX_EN             GPIO_SetBits(RS485_EN_GPIO,RS485_EN_Pin);
#define RX_EN             GPIO_ResetBits(RS485_EN_GPIO,RS485_EN_Pin);
/* ----------------------- static functions ---------------------------------*/
void prvvUARTTxReadyISR( void );
void prvvUARTRxISR( void );
void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	if (xRxEnable)  //接收使能
	{
		USART_ITConfig(USE_USART, USART_IT_RXNE, ENABLE);  //使能接收中断
		RX_EN;//接收
	}
	else  //失能
	{
		USART_ITConfig(USE_USART, USART_IT_RXNE, DISABLE);  //失能接收中断
		TX_EN //恢复发送
	}
	
	if (xTxEnable)  //发送使能
	{
		USART_ITConfig(USE_USART, USART_IT_TC, ENABLE);  //使能
		TX_EN //发送
	}
	else  //失能
	{
		USART_ITConfig(USE_USART, USART_IT_TC, DISABLE);  //失能
		RX_EN//设置接收
	}
}

BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	(void)ucPORT;  //不修改串口号
	(void)ucDataBits;  //不修改数据位长度
	(void)eParity;  //不修改检验格式
	
	RCC_AHB1PeriphClockCmd(USART_Tx_GPIO_CLK|USART_Rx_GPIO_CLK|RS485_EN_GPIO_CLK, ENABLE);
	RCC_APB1PeriphClockCmd(USE_USART_CLK, ENABLE);
	
	//
	//管脚复用
	//
	GPIO_PinAFConfig(USART_Tx_GPIO, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(USART_Rx_GPIO, GPIO_PinSource11, GPIO_AF_USART3);
	
	//
	//发送管脚 PA.02
	//接收管脚 PA.03
	//
	GPIO_InitStructure.GPIO_Pin   = USART_Tx_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(USART_Tx_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = USART_Rx_Pin;
	GPIO_Init(USART_Rx_GPIO, &GPIO_InitStructure);
	//
	//485芯片发送接收控制管脚
	//
	GPIO_InitStructure.GPIO_Pin   = RS485_EN_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(RS485_EN_GPIO, &GPIO_InitStructure);
	
	//
	//配置串口参数
	//
	USART_InitStructure.USART_BaudRate            = ulBaudRate;  //只修改波特率
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Parity              = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USE_USART, &USART_InitStructure);
	//
	//使能串口
	//
	USART_Cmd(USE_USART, ENABLE);
	
	//
	//配置中断优先级
	//
	NVIC_InitStructure.NVIC_IRQChannel                   =  USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	return TRUE;
}

BOOL xMBPortSerialPutByte( CHAR ucByte )
{   
	USART_SendData(USE_USART, ucByte);  //发送一个字节
	return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
	*pucByte = USART_ReceiveData(USE_USART);  //接收一个字节
	return TRUE;
}
void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );//在发送完成中断调用该函数
}
void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );//在接收中断函数调用该函数
}

/**
  *****************************************************************************
  * @Name   : 串口中断服务函数
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void  USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USE_USART, USART_IT_RXNE) == SET)  //接收中断
	{
		prvvUARTRxISR();
		USART_ClearITPendingBit(USE_USART, USART_IT_RXNE);
	}
	
	if (USART_GetITStatus(USE_USART, USART_IT_TC) == SET)  //发送中断
	{
		prvvUARTTxReadyISR();
		USART_ClearITPendingBit(USE_USART, USART_IT_TC);
	}
}
