#ifndef __NRF24L01_H__
#define __NRF24L01_H__
#include "user_config.h"
/*--------NRF24L01寄存器声明--------*/
#include "NRF24L01_adress.h"
/*------------------END-------------*/
/*--------NRF24L01的IO口宏定义------*/
#define NRF24L01_CS_GPIO  GPIOF
#define NRF24L01_CS_Pin   GPIO_Pin_7
#define NRF24L01_CS_GPIO_RCC RCC_AHB1Periph_GPIOF

#define NRF24L01_CE_GPIO  GPIOA
#define NRF24L01_CE_Pin   GPIO_Pin_4
#define NRF24L01_CE_GPIO_RCC RCC_AHB1Periph_GPIOA

#define NRF24L01_IRQ_GPIO GPIOH
#define NRF24L01_IRQ_Pin  GPIO_Pin_7
#define NRF24L01_IRQ_GPIO_RCC RCC_AHB1Periph_GPIOH

#define NRF24L01_SPI_GPIO GPIOB
#define NRF24L01_SCK_Pin  GPIO_Pin_3
#define NRF24L01_MOSI_Pin GPIO_Pin_5
#define NRF24L01_MISO_Pin GPIO_Pin_4
#define NRF24L01_SPI_GPIO_RCC  RCC_AHB1Periph_GPIOB
/*------------------END-------------*/
/*------NRF24L01引脚操作宏定义------*/
#define CLR_CE   GPIO_ResetBits(NRF24L01_CE_GPIO,NRF24L01_CE_Pin)
#define SET_CE   GPIO_SetBits(NRF24L01_CE_GPIO,NRF24L01_CE_Pin)
#define CLR_CS   GPIO_ResetBits(NRF24L01_CS_GPIO,NRF24L01_CS_Pin)
#define SET_CS   GPIO_SetBits(NRF24L01_CS_GPIO,NRF24L01_CS_Pin)
#define Read_IRQ GPIO_ReadInputDataBit(NRF24L01_IRQ_GPIO,NRF24L01_IRQ_Pin)
/*------------------END-------------*/
/*-----NRF24L01的SPI接口宏定义------*/
#define NRF24L01_SPI     SPI3
#define NRF24L01_SPI_RCC RCC_APB1Periph_SPI3
/*------------------END-------------*/
/*--------------函数宏定义----------*/
#define SPI_Write(data)  SPI_Wrie_Read_data(data)
#define SPI_Read         SPI_Wrie_Read_data(NOP)
/*------------------END-------------*/
/*----------基本函数声明------------*/
void NRF24L01_GPIO_Config(void);
void NRF24L01_SPI_Config(void);
static u8 SPI_Wrie_Read_data(u8 data);
static void NRF24L01_delay(u32 i);
/*-----------------END--------------*/
/*----------相关配置函数------------*/
u8   SPI_Wtite_Buf(u8 reg,u8 *pBuf,u8 longth);
u8   SPI_Read_Buf(u8 reg,u8 *pBuf,u8 longth);
u8   SPI_Write_Reg(u8 reg,u8 data);
u8   SPI_Read_Reg(u8 reg);
void NRF24L01_RX_Mode_Config(void);
void NRF24L01_TX_Mode_Config(void);
u8   NRF24L01_Check(void);
u8   NRF24L01_TX_Data(u8 *txbuf);
u8   NRF24L01_RX_Data(u8 *rxbuf);
/*-----------------END--------------*/
/*----------其它宏定义--------------*/
#define TX_ADR_WIDTH 	  5 //发送数据地址长度
#define TX_PLOAD_WIDTH  4 //发送数据的长度
#define RX_ADR_WIDTH    5 //接收数据的地址长度
#define RX_PLOAD_WIDTH  4 //接收数据的长度
#define CHANAL          40
/*-----------------END--------------*/
#endif
