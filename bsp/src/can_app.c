#include "can_app.h" 
typedef  void (*pFunction)(void);
#define READ_MAX  256   //每次读取数据的最大长度,16位数据
u8 read_temp[READ_MAX];
u32 read_addr       = (u32)0x00;//读取数据起始地址
u32 read_len        = (u32)0x00;//读取数据长度
/*
相关设备信息
包括当前芯片型号,当前设备地址,
当前设备固件类型,当前设备固件更改日期,当前设备版本号

*/
Device_INFO DEVICE_INFO =
{
 
 .FW_Version.bits.Version       = 01,
 .FW_Version.bits.date          = 03,
 .FW_Version.bits.month         = 06,
 .FW_Version.bits.year          = 2018,
 .FW_TYPE.bits.FW_TYPE          = CAN_BL_APP,
 .Device_addr.bits.reserve      = 0x00,
 .FW_TYPE.bits.Chip_Value       = STM32F407IGT6,
 .Device_addr.bits.Device_addr  = DEVICE_ADDR,
};

//命令参数必须跟上位机软件的命令参数一致
CBL_CMD_LIST CMD_List = 
{ 
	.Read        = 0x0A, //读取flash数据
	.Erase       = 0x00, //擦除APP区域数据
	.Write       = 0x02,//以多字节形式写数据
	.Check       = 0x03,//检测节点是否在线，同时返回固件信息
	.Excute      = 0x05,//执行固件
	.CmdFaild    = 0x09,//命令执行失败
	.WriteInfo   = 0x01,//设置多字节写数据相关参数(写起始地址,数据量)
	.CmdSuccess  = 0x08,//命令执行成功
	.SetBaudRate = 0x04,//设置节点波特率
};
/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }

  return sector;
}
/**
  * @brief  将数据烧写到指定地址的Flash中 。
  * @param  Address Flash 起始地址。
  * @param  Data 数据存储区起始地址。
  * @param  DataNum 数据字节数。
  * @retval 数据烧写状态。
  */
FLASH_Status CAN_BOOT_ProgramDatatoFlash(uint32_t StartAddr,uint8_t *pData,uint32_t DataNum) 
{
  FLASH_Status FLASHStatus=FLASH_COMPLETE;

  uint32_t *pDataTemp=(uint32_t *)pData;
  uint32_t i;

  if(StartAddr<APP_EXE_FLAG_START_ADDR)
	{
    return FLASH_ERROR_PGS;
  }
  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 

  for(i=0;i<(DataNum>>2);i++)
  {
    FLASHStatus = FLASH_ProgramWord(StartAddr, *pDataTemp);
    if (FLASHStatus == FLASH_COMPLETE)
		{
      StartAddr += 4;
      pDataTemp++;
    }
		else
		{ 
      return FLASHStatus;
    }
  }
  return	FLASHStatus;
}
/**
  * @brief  擦出指定扇区区间的Flash数据 。
  * @param  StartPage 起始扇区地址
  * @param  EndPage 结束扇区地址
  * @retval 扇区擦出状态  
  */
FLASH_Status CAN_BOOT_ErasePage(uint32_t StartAddr,uint32_t EndAddr)
{
	FLASH_Status FLASHStatus=FLASH_COMPLETE;
	uint32_t StartSector, EndSector;
	uint32_t SectorCounter=0;
	FLASH_Unlock();
	/* Get the number of the start and end sectors */
	StartSector = GetSector(StartAddr);
	EndSector = GetSector(EndAddr);

	if(StartAddr<APP_EXE_FLAG_START_ADDR)
	{
		return FLASH_ERROR_PGS;
	}
  /* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_OPERR|FLASH_FLAG_WRPERR|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	for (SectorCounter = StartSector; SectorCounter <= EndSector; SectorCounter += 8)
	{ 
		FLASHStatus = FLASH_EraseSector(SectorCounter, VoltageRange_3);
		if (FLASHStatus != FLASH_COMPLETE)
		{
			FLASH_Lock();
			return FLASHStatus;
		}
	}
	FLASH_Lock();
	return FLASHStatus;
}

/**
  * @brief  从指定的Flash区读取一个字的数据
  * @param  Address 开始读取数据的地址。
	* @param  pData 数据储存地址。
  * @retval 程序跳转状态。
  */
uint16_t CAN_BOOT_GetAddrData(void)
{
  return Read_CAN_Address();
}
/**
  * @brief  控制程序跳转到指定位置开始执行
  * @param  Addr 程序执行地址
  * @retval 程序跳转状态
  */
void CAN_BOOT_JumpToApplication(__IO uint32_t Addr)
{
	pFunction Jump_To_Application;
	__IO uint32_t JumpAddress; 
	if (((*(__IO uint32_t*)Addr) & 0x2FFE0000 ) == 0x20000000)
	{  
		JumpAddress = *(__IO uint32_t*) (Addr + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		__set_PRIMASK(1);//关闭所有中断 
		__set_MSP(*(__IO uint32_t*)Addr);
		Jump_To_Application();
	}
}


/**
  * @brief  执行主机下发的命令
  * @param  pRxMessage CAN总线消息
  * @retval 无
  */
void CAN_BOOT_ExecutiveCommand(CanRxMsg *pRxMessage)
{
	CanTxMsg TxMessage; 
	uint8_t  can_cmd = (pRxMessage->ExtId)&CMD_MASK;//ID的bit0~bit3位为命令码
	uint16_t can_addr = (pRxMessage->ExtId >> CMD_WIDTH);//ID的bit4~bit15位为节点地址
	uint32_t BaudRate = (uint32_t)0x00;
	uint32_t exe_type = (uint32_t)0x00;
	//判断接收的数据地址是否和本节点地址匹配，若不匹配则直接返回，不做任何事情
	if((can_addr!=DEVICE_ADDR)&&(can_addr!=0))
	{
		return;
	}
	TxMessage.DLC = 0;
	TxMessage.ExtId = 0;
	TxMessage.IDE = CAN_Id_Extended;
	TxMessage.RTR = CAN_RTR_Data; 
  //CMD_List.SetBaudRate，设置节点波特率，具体波特率信息存储在Data[0]到Data[3]中
  //更改波特率后，适配器也需要更改为相同的波特率，否则不能正常通信
	if(can_cmd == CMD_List.SetBaudRate)
	{
		BaudRate = (pRxMessage->Data[0]<<24)|(pRxMessage->Data[1]<<16)|(pRxMessage->Data[2]<<8)|(pRxMessage->Data[3]<<0);
		CAN_Configuration(BaudRate);
		if(can_addr != 0x00)
		{
			TxMessage.ExtId = (CAN_BOOT_GetAddrData()<<CMD_WIDTH)|CMD_List.CmdSuccess;
			TxMessage.DLC = 0;
			delay_ms(20);
			CAN_WriteData(&TxMessage);
		}
		return;
	}
  //CMD_List.Check，节点在线检测
  //节点收到该命令后返回固件版本信息和固件类型，该命令在Bootloader程序和APP程序都必须实现
	if(can_cmd == CMD_List.Check)
	{
		if(can_addr != 0x00)
		{
			TxMessage.ExtId = (CAN_BOOT_GetAddrData()<<CMD_WIDTH)|CMD_List.CmdSuccess; 
			TxMessage.DLC = 8; 
			TxMessage.Data[0] = (u8)(DEVICE_INFO.FW_Version.all>>24);;//主版本号，两字节
			TxMessage.Data[1] = (u8)(DEVICE_INFO.FW_Version.all>>16);
			TxMessage.Data[2] = (u8)(DEVICE_INFO.FW_Version.all>>8);//次版本号，两字节
			TxMessage.Data[3] = (u8)(DEVICE_INFO.FW_Version.all>>0);
			TxMessage.Data[4] = (u8)(DEVICE_INFO.FW_TYPE.bits.FW_TYPE>>16);
			TxMessage.Data[5] = (u8)(DEVICE_INFO.FW_TYPE.bits.FW_TYPE>>8);
			TxMessage.Data[6] = (u8)(DEVICE_INFO.FW_TYPE.bits.FW_TYPE>>0);
			TxMessage.Data[7] = (u8)(DEVICE_INFO.FW_TYPE.bits.Chip_Value>>0); 
			CAN_WriteData(&TxMessage);
		}
		return;
	}
		//cmd_list.read,读取flash数据,
	//该命令是用于读取内部存储器数据
	//该命令在Bootloader和APP程序中国必须实现
		if(can_cmd == CMD_List.Read)
		{
			if(pRxMessage->DLC != 8)
			{
				TxMessage.DLC     = 0x02;
				TxMessage.ExtId   = (DEVICE_INFO.Device_addr.bits.Device_addr<<CMD_WIDTH)|CMD_List.CmdFaild;
				TxMessage.Data[0] = (u8)CMD_List.Read;//主版本号，两字节
				TxMessage.Data[1] = (u8)MSG_DATA_LEN_ERROR;
				CAN_WriteData(&TxMessage);
				return;
			}
			read_addr =  (((u32)(pRxMessage->Data[0])&0xFFFFFFFF)<<0x18)|\
									 (((u32)(pRxMessage->Data[1])&0x00FFFFFF)<<0x10)|\
									 (((u32)(pRxMessage->Data[2])&0x0000FFFF)<<0x08)|\
									 (((u32)(pRxMessage->Data[3])&0x000000FF)<<0x00);
			read_len  =  (((u32)(pRxMessage->Data[4])&0xFFFFFFFF)<<0x18)|\
									 (((u32)(pRxMessage->Data[5])&0x00FFFFFF)<<0x10)|\
									 (((u32)(pRxMessage->Data[6])&0x0000FFFF)<<0x08)|\
									 (((u32)(pRxMessage->Data[7])&0x000000FF)<<0x00);
			if(read_len > READ_MAX)
			{
				TxMessage.DLC                              = 0x02;
				TxMessage.ExtId                  = (DEVICE_INFO.Device_addr.bits.Device_addr<<CMD_WIDTH)|CMD_List.CmdFaild;
				TxMessage.Data[0] = (u8)CMD_List.Read;//主版本号，两字节
				TxMessage.Data[1] = (u8)READ_LEN_ERROR;
				CAN_WriteData(&TxMessage);
				return;
			}
			/*
			u16 read_len_temp  = 0;
			if(read_len%2 == 0)//因为每次只能读取N个字
			{
				read_len_temp = read_len;
			}
			else
			{
				read_len_temp = read_len+1;
			}
			read_len_temp = read_len_temp>>1;
			*/
			__set_PRIMASK(1);
			STM32F4_Read_Flash(read_addr,read_temp,read_len);
			__set_PRIMASK(0);
			int i =0; 
			int data_index = 0; 
			while(data_index < read_len)
			{
				int temp;
				temp = read_len - data_index;
				if (temp >= 8)
				{
					TxMessage.DLC             = 0x08;
					TxMessage.ExtId = (DEVICE_INFO.Device_addr.bits.Device_addr<<CMD_WIDTH)|CMD_List.CmdSuccess;
					for (i = 0; i < TxMessage.DLC; i++)
					{
						TxMessage.Data[i] = read_temp[data_index];
						data_index++;
					}

				}
				else
				{
					TxMessage.DLC             = temp;
					TxMessage.ExtId = (DEVICE_INFO.Device_addr.bits.Device_addr<<CMD_WIDTH)|CMD_List.CmdSuccess;
					for (i = 0; i < TxMessage.DLC; i++)
					{
						TxMessage.Data[i] = read_temp[data_index];
						data_index++;
					}
				}
				CAN_WriteData(&TxMessage);
			}
			data_index = 0;
			return;
		}
		
	
  //CMD_List.Excute，控制程序跳转到指定地址执行
  //该命令在Bootloader和APP程序中都必须实现
  if(can_cmd == CMD_List.Excute)
  {
    exe_type = (pRxMessage->Data[0]<<0x10)|\
			   (pRxMessage->Data[1]<<0x08)|\
			   (pRxMessage->Data[2]<<0x00);
    if(exe_type == CAN_BL_BOOT)
	{
      FLASH_Unlock();
      CAN_BOOT_ErasePage(APP_EXE_FLAG_ADDR,APP_EXE_FLAG_ADDR);//擦除写入到Flash中的APP执行标志，复位运行后，即可执行Bootloader程序
      FLASH_Lock();
      __set_PRIMASK(1);//关闭所有中断
      NVIC_SystemReset();
    }
    return;
  }
}
void STM32F4_Read_Flash(u32 adress,u8 *pdata,u32 len)
{
	int cnt = 0;
	for(cnt = 0;cnt < len;cnt++)
	{
		pdata[cnt] = *(vu8*)adress;
		adress=adress+1;//因为是一个全字所以加4，半字加2
	}
}
/*********************************END OF FILE**********************************/

