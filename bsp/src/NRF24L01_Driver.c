#include "NRF24L01.h"
 u8 RX_BUF[RX_PLOAD_WIDTH];
 u8 TX_BUF[TX_PLOAD_WIDTH];	
 u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};  
 u8 RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};
/*
函数名:void NRF24L01_GPIO_Config(void);
功能:配置NRF24L01的IO接口;
书参数:无;
返回值:无;
调用:外部函数调用;
*/
static void NRF24L01_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_init;
	RCC_AHB1PeriphClockCmd(NRF24L01_CE_GPIO_RCC|NRF24L01_CS_GPIO_RCC|NRF24L01_IRQ_GPIO_RCC|NRF24L01_SPI_GPIO_RCC,ENABLE);//使能时钟
	GPIO_PinAFConfig(NRF24L01_SPI_GPIO,GPIO_PinSource3,GPIO_AF_SPI3);
	GPIO_PinAFConfig(NRF24L01_SPI_GPIO,GPIO_PinSource4,GPIO_AF_SPI3);
	GPIO_PinAFConfig(NRF24L01_SPI_GPIO,GPIO_PinSource5,GPIO_AF_SPI3);
	/********配置NRF24L01的SPI通信引脚*********/
	GPIO_init.GPIO_Mode = GPIO_Mode_AF;
	GPIO_init.GPIO_OType = GPIO_OType_PP;
	GPIO_init.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_init.GPIO_Pin = NRF24L01_MOSI_Pin|NRF24L01_SCK_Pin|NRF24L01_MISO_Pin;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NRF24L01_SPI_GPIO,&GPIO_init);
	/******配置NRF24L01的CE,CS,IRQ引脚*******/
	GPIO_init.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_init.GPIO_Pin  = NRF24L01_CE_Pin;
	GPIO_Init(NRF24L01_CE_GPIO,&GPIO_init);
	GPIO_init.GPIO_Pin = NRF24L01_CS_Pin;
	GPIO_Init(NRF24L01_CS_GPIO,&GPIO_init);
	GPIO_init.GPIO_Mode = GPIO_Mode_IN;
	GPIO_init.GPIO_Pin = NRF24L01_IRQ_Pin;
	GPIO_Init(NRF24L01_IRQ_GPIO,&GPIO_init);
}
/*
函数名:void NRF24L01_SPI_Config(void);
功能:配置SPI的工作模式;
输入参数:无;
返回值:无;
调用:外部函数调用;
*/
void NRF24L01_SPI_Config(void)
{
	SPI_InitTypeDef SPI_init;
	NRF24L01_GPIO_Config();
	RCC_APB1PeriphClockCmd(NRF24L01_SPI_RCC,ENABLE);
	SPI_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_init.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_init.SPI_CPOL = SPI_CPOL_Low;
	SPI_init.SPI_CRCPolynomial = 7;
	SPI_init.SPI_DataSize  = SPI_DataSize_8b;
	SPI_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_init.SPI_FirstBit  = SPI_FirstBit_MSB;
	SPI_init.SPI_Mode = SPI_Mode_Master;
	SPI_init.SPI_NSS  = SPI_NSS_Soft;
	SPI_Init(NRF24L01_SPI,&SPI_init);
	SPI_Cmd(NRF24L01_SPI,ENABLE);
}
/*
函数名:static u8 SPI_Wrie_Read_data(u8 data);
功能:通过SPI接口读写数据;
输入参数: u8 data;
返回值:u8 retun_data;
调用:内部函数调用;
*/
static u8 SPI_Wrie_Read_data(u8 data)
{
	u8 retun_data;
	while(SPI_I2S_GetFlagStatus(NRF24L01_SPI,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(NRF24L01_SPI,data);
	while(SPI_I2S_GetFlagStatus(NRF24L01_SPI,SPI_I2S_FLAG_RXNE) == RESET);
	retun_data = SPI_I2S_ReceiveData(NRF24L01_SPI);
	return retun_data;
}
/*
函数名:u8 SPI_Wtite_Buf(u8 reg,u8 *pBuf,u8 longth);
功能:向无线模块写入数据;
输入参数:u8 reg:寄存器地址;
         u8 *pBuf:需要写入的数据;
         u8 longth:需要写入的数据长度;
返回值:状态寄存器的值;
调用:外部函数调用;
*/
u8 SPI_Wtite_Buf(u8 reg,u8 *pBuf,u8 longth)
{
	u8 i,status;
	CLR_CE;
	CLR_CS;
	status = SPI_Write(reg);
	for(i = 0;i < longth; i++)
	{
		SPI_Write(*pBuf++);
	}
	SET_CS;
	return status;
}
/*
函数名:u8 SPI_Read_Buf(u8 reg,u8 *pBuf,u8 longth);
功能:从无线模块读取数据;
输入参数:u8 reg:寄存器地址;
         u8 *pBuf:需要写入的数据;
         u8 longth:需要写入的数据长度;
返回值:状态寄存器的值;
调用:外部函数调用;
*/
u8 SPI_Read_Buf(u8 reg,u8 *pBuf,u8 longth)
{
	u8 i,status;
	CLR_CE;
	CLR_CS;
	status =SPI_Write(reg);
	for(i = 0; i < longth; i++)
	{
		pBuf[i] = SPI_Read;
	}
	SET_CS;
	return status;
}
/*
函数名:u8 SPI_Write_Reg(u8 reg,u8 data);
功能:向NRF24L01寄存器写入数据;
输入参数: u8 reg:寄存器地址;
					u8 data:写入寄存器的数据;
返回值:状态寄存器的值;
调用:外部函数调用;
*/
u8 SPI_Write_Reg(u8 reg,u8 data)
{
	u8 status;
	CLR_CE;
	CLR_CS;
	status = SPI_Write(reg);
	SPI_Write(data);
	SET_CS;
	return status;
}
/*
函数名:u8 SPI_Read_Reg(u8 reg);
功能:向NRF24L01寄存器写入数据;
输入参数: u8 reg:寄存器地址;
返回值:读取寄存器的值;
调用:外部函数调用;
*/
u8 SPI_Read_Reg(u8 reg)
{
	u8 reg_data;
	CLR_CE;
	CLR_CS;
	SPI_Write(reg);
	reg_data = SPI_Read;
	SET_CS;
	return reg_data;
}
/*
函数名:void NRF24L01_RX_Mode_Config(void);
功能:将NRF24L01配置为接收模式;
输入参数:无;
返回值:无;
调用:外部函数调用;
*/
void NRF24L01_RX_Mode_Config(void)
{
	CLR_CE;
	SPI_Wtite_Buf(NRF24L01_Write_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); // 写入RX节点的地址;
	SPI_Write_Reg(NRF24L01_Write_REG+EN_AA,0x01);                         //使能通道0自动应答;
	SPI_Write_Reg(NRF24L01_Write_REG+EN_RXADDR,0x01);											//使能通道0的接收地址;
	SPI_Write_Reg(NRF24L01_Write_REG+RF_CH,CHANAL);												//设置RF通信频率;
	SPI_Write_Reg(NRF24L01_Write_REG+RX_PW_P0,RX_PLOAD_WIDTH);            //选择通道0的有效数据宽度;
	SPI_Write_Reg(NRF24L01_Write_REG+RF_SETUP,0x0f);       								//设置TX发射参数:0dB增益,2Mps,低噪声增益开;
	SPI_Write_Reg(NRF24L01_Write_REG+CONFIG,0x0f);                        //配置基本工作模式:PWR_UP,EN_CRC,16Bit_CRC,接收模式;
	SET_CE;
}
/*
函数名:void NRF24L01_TX_Mode_Config(void);
功能:配置NRF24L01为发送模式;
输入参数:无;
返回值:无;
调用:外部函数调用;
*/
void NRF24L01_TX_Mode_Config(void)
{
	CLR_CE;
	SPI_Wtite_Buf(NRF24L01_Write_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    // 写入TX节点的地址;
	SPI_Wtite_Buf(NRF24L01_Write_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,目的是为了使能ACK应答
	SPI_Write_Reg(NRF24L01_Write_REG+EN_AA,0x01);                         //使能通道0自动应答;
	SPI_Write_Reg(NRF24L01_Write_REG+EN_RXADDR,0x01);											//使能通道0的接收地址;
	SPI_Write_Reg(NRF24L01_Write_REG+SETUP_RETR,0x0A);                    //设置自动重发时间间隔:500us+86us,最大重发次数:10;
	SPI_Write_Reg(NRF24L01_Write_REG+RF_CH,CHANAL);												//设置RF通信频率;
	SPI_Write_Reg(NRF24L01_Write_REG+RF_SETUP,0x0f);       								//设置TX发射参数:0dB增益,2Mps,低噪声增益开;
	SPI_Write_Reg(NRF24L01_Write_REG+CONFIG,0x0e);                        //配置基本工作模式:PWR_UP,EN_CRC,16Bit_CRC,发送模式;
	SET_CE;
	NRF24L01_delay(0xffff);
}
/*
函数名:u8 NRF24L01_Check(void);
功能:检测NRF24L01与MCU是否正常连接;
输入参数:无;
返回值:0:表示连接成功
       1:表示连接失败
调用:外部函数调用;
*/
u8 NRF24L01_Check(void)
{
	u8 buf[5] = {0x0A,0x0A,0x0A,0x0A,0x0A};
	u8 buf1[5];
	u8 i;
	SPI_Wtite_Buf(NRF24L01_Write_REG+TX_ADDR,buf,5);
	SPI_Read_Buf(TX_ADDR,buf1,5);
	for( i = 0; i <5; i++)
	{
		if(buf1[i] != 0x0A)
			break;
	}
	if( i == 5)
		return 0 ; //表示成功连接;
	else
		return 1;  //表示连接失败;
}
/*
函数名:u8 NRF24L01_TX_Data(u8 *txbuf);
功能:通过NRF24L01发送数据;
输入参数:u8 *txbuf:需要发送的数据;
返回值:状态寄存器的值:u8 status;
调用:外部函数调用;
*/
u8 NRF24L01_TX_Data(u8 *txbuf)
{
	u8 status;
	u16 cnt = 0;
	CLR_CE;
	SPI_Wtite_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);
	SET_CE;
	  while(Read_IRQ != 0)
	{
		cnt++;
		NRF24L01_delay(5000);
		if(cnt > 5000)
		{
			break;
		}
	}	                            //等待中断引脚
	status = SPI_Read_Reg(STATUS);                      //读取标志位
	SPI_Write_Reg(NRF24L01_Write_REG+STATUS,status);    //清除标志位
	SPI_Write_Reg(FLUSH_TX,NOP);
	if(status&MAX_RT)
	{
		return MAX_RT;
	}
	else if(status&TX_DS)
	{
		return TX_DS;
	}
	else
	{
		return ERROR;
	}
}
/*
函数名:u8 NRF24L01_RX_Data(u8 *rxbuf);
功能:通过NRF24L01接收数据;
输入参数:u8 *rxbuf:需要发送的数据;
返回值:状态寄存器的值:u8 status;
调用:外部函数调用;
*/
u8 NRF24L01_RX_Data(u8 *rxbuf)
{
	u8 status;
	u16 cnt = 0;
	SET_CE;
  while(Read_IRQ != 0)
	{
		cnt++;
		NRF24L01_delay(5000);
		if(cnt > 5000)
		{
			break;
		}
	}
	CLR_CE;
	status = SPI_Read_Reg(STATUS);                      //读取标志位
	SPI_Write_Reg(NRF24L01_Write_REG+STATUS,status);    //清除标志位
	if(status&RX_DR)
	{
		SPI_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);
		SPI_Write_Reg(FLUSH_RX,NOP);
		return RX_DR;
	}
	else
	{
		return ERROR;
	}
}
/*
函数名:static void NRF24L01_delay(u32 i);
功能描述:用于NRF24L01延时;
输入参数: u32 i;延时长度
返回值:无;
调用:内部调用;
*/
static void NRF24L01_delay(u32 i)
{
	while(i)
	{
		i--;
	}
	
}
