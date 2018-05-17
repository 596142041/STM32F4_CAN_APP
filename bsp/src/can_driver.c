/**
  ******************************************************************************
  * @file    can_driver.c
  * $Author: wdluo  
  * $Revision: 17  
  * $Date:: 2012-07-06 11:16:48 +0800  
  * @brief   CAN总线收发相关函数.
  ******************************************************************************
  * @attention
  ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can_app.h"
#define CAN_Tx_Port      GPIOH
#define CAN_Tx_Pin       GPIO_Pin_13
#define CAN_Tx_Port_CLK  RCC_AHB1Periph_GPIOH 
#define CAN_Rx_Port      GPIOI
#define CAN_Rx_Pin       GPIO_Pin_9
#define CAN_Rx_Port_CLK  RCC_AHB1Periph_GPIOI
/* Private typedef -----------------------------------------------------------*/
typedef  struct {
  unsigned char   CAN_SJW;
  unsigned char   CAN_BS1;
  unsigned char   CAN_BS2;
  unsigned short int  CAN_Prescaler;
  unsigned long  int  BaudRate;
 
} CAN_BaudRate;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
CanRxMsg CAN1_RxMessage;
volatile uint8_t CAN1_CanRxMsgFlag=0;//接收到CAN数据后的标志
CAN_BaudRate  CAN_BaudRateInitTab[27]=  // CLK=42MHz
{     
   {CAN_SJW_1tq,CAN_BS1_12tq,CAN_BS1_8tq,0x02, 1000000},     // 1M
   {CAN_SJW_1tq,CAN_BS1_12tq,CAN_BS1_8tq,0x04, 500000},     // 500K
   {CAN_SJW_1tq,CAN_BS1_12tq,CAN_BS1_8tq,0x05, 400000},     // 400K
   {CAN_SJW_1tq,CAN_BS1_12tq,CAN_BS1_8tq,0x08, 250000},    // 250K
   {CAN_SJW_1tq,CAN_BS1_12tq,CAN_BS1_8tq,0x0A, 200000},    // 200K
   {CAN_SJW_1tq,CAN_BS1_12tq,CAN_BS1_8tq,0x10, 125000},     // 500K
   {CAN_SJW_1tq,CAN_BS1_12tq,CAN_BS1_8tq,0x14, 100000},    // 100K
   {CAN_SJW_1tq,CAN_BS1_12tq,CAN_BS1_8tq,0x28, 50000},    // 50K 
};
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  通过波特率的值获取波特率参数表索引值
  * @param  BaudRate CAN总线波特率，单位为bps
  * @retval 波特率参数表索引值
  */
uint32_t CAN_GetBaudRateNum(uint32_t BaudRate)
{
	int i;
	for(i = 0;i<27;i++)
	{
		if(CAN_BaudRateInitTab[i].BaudRate == BaudRate)
		{
			return i;
		}
		
	}
	return 0;
}
/**
  * @brief  CAN引脚配置
  * @param  None
  * @retval None
  */
void CAN_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_init;
	RCC_AHB1PeriphClockCmd(CAN_Tx_Port_CLK|CAN_Rx_Port_CLK,ENABLE);
	GPIO_PinAFConfig(CAN_Tx_Port,GPIO_PinSource13,GPIO_AF_CAN1);
	GPIO_PinAFConfig(CAN_Rx_Port,GPIO_PinSource9,GPIO_AF_CAN1);
	GPIO_init.GPIO_Mode = GPIO_Mode_AF;
	GPIO_init.GPIO_OType  = GPIO_OType_PP;
	GPIO_init.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Pin = CAN_Tx_Pin;
	GPIO_Init(CAN_Tx_Port,&GPIO_init);
	GPIO_init.GPIO_Pin = CAN_Rx_Pin;
	GPIO_Init(CAN_Rx_Port,&GPIO_init); 
}
/**
  * @brief  CAN接收中断配置
  * @param  None
  * @retval None
  */
void CAN_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	// Enable CAN1 RX0 interrupt IRQ channel
	NVIC_InitStructure.NVIC_IRQChannel                   = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  配置CAN接收过滤器
  * @param  FilterNumber 过滤器号
  * @param  can_addr CAN节点地址，该参数非常重要，同一个CAN总线网络其节点地址不能重复
  * @retval None
  */
void CAN_ConfigFilter(uint8_t FilterNumber,uint16_t can_addr)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure; 
	u32 addr_temp;
	addr_temp = can_addr<<CMD_WIDTH;//将输入的ID右移四位，这样还原的最最终的ID位数
	//设置CAN接收过滤器
	CAN_FilterInitStructure.CAN_FilterNumber=FilterNumber;//过滤器1
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;//掩码模式,屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;//32bit模式
	CAN_FilterInitStructure.CAN_FilterIdHigh=addr_temp>>13;
	CAN_FilterInitStructure.CAN_FilterIdLow=((addr_temp<<3)&0xFFFF)|0x04;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=ADDR_MASK>>(13);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=((ADDR_MASK<<3)&0xFF87)|0x04;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//使能过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);	
}
/**
  * @brief  初始化CAN
  * @param  BaudRate CAN总线波特率
  * @retval None
  */
void CAN_Configuration(uint32_t BaudRate)
{
	CAN_InitTypeDef        CAN_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);  

	CAN_GPIO_Configuration();
	CAN_DeInit(CAN1); 
	CAN_StructInit(&CAN_InitStructure); 
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM      = DISABLE;
	CAN_InitStructure.CAN_ABOM      = DISABLE;
	CAN_InitStructure.CAN_AWUM      = DISABLE;
	CAN_InitStructure.CAN_NART      = DISABLE;
	CAN_InitStructure.CAN_RFLM      = DISABLE;
	CAN_InitStructure.CAN_TXFP      = ENABLE;
	CAN_InitStructure.CAN_Mode      = CAN_Mode_Normal; 
	CAN_InitStructure.CAN_BS1       = CAN_BaudRateInitTab[CAN_GetBaudRateNum(BaudRate)].CAN_BS1;
	CAN_InitStructure.CAN_BS2       = CAN_BaudRateInitTab[CAN_GetBaudRateNum(BaudRate)].CAN_BS2;
	CAN_InitStructure.CAN_SJW       = CAN_BaudRateInitTab[CAN_GetBaudRateNum(BaudRate)].CAN_SJW;
	CAN_InitStructure.CAN_Prescaler = CAN_BaudRateInitTab[CAN_GetBaudRateNum(BaudRate)].CAN_Prescaler;
	CAN_Init(CAN1,&CAN_InitStructure);
	//设置CAN接收过滤器
	CAN_ConfigFilter(0,0x00);//广播地址，接受广播命令
	CAN_ConfigFilter(1,CAN_BOOT_GetAddrData());//本节点真实地址
	//使能接收中断
	
		//CAN_NVIC_Configuration();
		//CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
}
/**
  * @brief  发送一帧CAN数据
  * @param  CANx CAN通道号
	* @param  TxMessage CAN消息指针
  * @retval None
  */
uint8_t CAN_WriteData(CanTxMsg *TxMessage)
{
  uint8_t   TransmitMailbox;   
  uint32_t  TimeOut=0;
  TransmitMailbox = CAN_Transmit(CAN1,TxMessage);
  while(CAN_TransmitStatus(CAN1,TransmitMailbox)!=CAN_TxStatus_Ok){
    TimeOut++;
    if(TimeOut > 10000000){
      return 1;
    }
  }
  return 0;
}
/**
  * @brief  CAN接收中断处理函数
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
  CAN_Receive(CAN1,CAN_FIFO0, &CAN1_RxMessage);
  CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
  CAN1_CanRxMsgFlag = 1;
}
/**
  * @brief  获取CAN节点地址，该函数根据自己的实际情况进行修改
  * @param  None
  * @retval None
  */
uint16_t Read_CAN_Address(void)
{
  return 0x132;//返回的地址值需要根据实际情况进行修改
}

/**
  * @brief  初始化配置获取CAN总线地址的GPIO引脚
  * @param  None
  * @retval None
  */
void CAN_Address_GPIO_Config(void)
{
  //根据自己的板子完成对应的程序
}

/*********************************END OF FILE**********************************/
