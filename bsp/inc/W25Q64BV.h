#ifndef __W25Q64BV_H__
#define __W25Q64BV_H__
#include "user_config.h"
//相关命令字节宏定义
#define CMD_AAI       0xAD  	/* AAI 连续编程指令(FOR SST25VF016B) */
#define CMD_WREN      0x06		/* 写使能命令 */
#define CMD_DISWR	    0x04		/* 禁止写, 退出AAI状态 */
#define CMD_EWRSR	    0x50		/* 允许写状态寄存器的命令 */
#define CMD_WRSR      0x01  	/* 写状态寄存器命令 */
//读取数据指令
#define CMD_READ      0x03  	/* 读数据区命令 */
#define CMD_RDSR      0x05		/* 读状态寄存器命令 */
#define CMD_RDID      0x9F		/* 读器件ID命令 */
//页写数据
#define CMD_PAGE_W    0x02
//芯片擦除指令
#define CMD_SE4k      0x20		/* 4kb擦除扇区命令 */
#define CMD_SE32k     0x52
#define CMD_SE64k     0xD8
#define CMD_Chip      0xC7		/* 整片擦除命令 */
#define CMD_CS        0x75   //擦除暂停指令
//其它控制指令
#define Power_Down     0xB9
//空指令用于读取数据使用
#define DUMMY_BYTE    0xFF		/* 哑命令，可以为任意值，用于读操作 */
//状态寄存器1中的相关位的描述
//状态寄存器1
#define WIP_FLAG      0x01		//* 状态寄存器中的正在编程标志（WIP) 
#define WEL_FLAG      0x02
//SPI接口宏定义
#define W25Q64_SPI    SPI3
#define W25Q64SPI_CLK RCC_APB1Periph_SPI3
//使用的IO口定义
#define W25Q64_CS_GPIO  GPIOF 
#define W25Q64_CS_Pin		GPIO_Pin_8
#define W25Q64_GPIO_RCC RCC_AHB1Periph_GPIOF
#define W25Q64_SPI_GPIO GPIOB
#define W25Q64_SCK_Pin  GPIO_Pin_3
#define W25Q64_MOSI_Pin GPIO_Pin_5
#define W25Q64_MISO_Pin GPIO_Pin_4
#define W25Q64_SPI_GPIO_RCC  RCC_AHB1Periph_GPIOB
#define W25Q64_CS_CLR   GPIO_ResetBits(W25Q64_CS_GPIO,W25Q64_CS_Pin)
#define W25Q64_CS_SET   GPIO_SetBits(W25Q64_CS_GPIO,W25Q64_CS_Pin) 
//常用SPI Flash ID
enum
{
	SST25VF016B_ID = 0xBF2541,
	MX25L1606E_ID  = 0xC22015,
	W25Q64BV_ID    = 0xEF4017
};
typedef struct
{
	
	uint32_t ChipID;		/* 芯片ID */
	char ChipName[16];		/* 芯片型号字符串，主要用于显示 */
	uint32_t TotalSize;		/* 总容量 */
	uint16_t PageSize;		/* 页面大小 */
	uint16_t device_id;
}SPI_FLASH;
/*******************************
*结构体:SECTO_INF
*描述:表示扇区相关信息,包括当前地址所处的扇区及其偏移地址
*参数:
*			unsigned short int sector_num 当前地址所处的扇区数
*			unsigned short int sector_offset 当前地址所处扇区的偏移地址
*/

typedef struct
{
	unsigned short int sector_num;
	unsigned short int sector_offset;
}SECTO_INF;
extern SPI_FLASH W25Q64;//声明一个SPI FLASH结构体
static void SPIFLAH_GPIO_Config(void);
static void SPIFLASH_SPI_Config(void);
static u8 SPI_send_data(u8 data);//发送一个字节数据
void SPIFLASH_Init(void);//SPI Flash 硬件接口初始化
void SPI_FLASH_ReadID(SPI_FLASH *SPI_flash);//读取器件ID信息,
void SPI_FLASH_ReadDevice_ID(SPI_FLASH *SPI_flash);//读取器件ID信息,
//以下函数是写入函数
static void SPI_FLASH_Write_Enable(void);//写使能
static void SPI_FLASH_Write_Disable(void);//关闭写
static void WaiForWriteEnd(void);//等待写完成
static void Write_Status(u8 status_data);//写入状态寄存器
//擦除相关函数
void Sector_4kb_Erase(u32 addr);//擦除4kb的扇区
void Sector_32kb_Erase(u32 addr);//擦除32Kb的扇区
void Sector_64kb_Erase(u32 addr);//擦除64KB的扇区
void Chip_Erase(void);//擦除整片
//写入数据
void Write_Page(u32 addr,u8 *data,u16 len);//整页的写入,因此len必须是256的整数倍
void SPI_Flash_WR(unsigned long int addr,unsigned long int leng,unsigned char *data);//写入任意长度的数据
//读取数据;
void Read_Buffer(u32 addr,u16 len,u8 *data);//读取任意长度的数据
#endif
