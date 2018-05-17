/*----------------------------------------------------------------------------*/
#include "hex_to_bin.h"
#include "stdio.h"
FIL fp_hex;
FIL fp_bin;
FATFS fatfs;
UINT read_cnt,write_cnt;
FSIZE_t hex_size,bin_size;
FRESULT res;
PACK_INFO pack_info;
unsigned char read_buf[128];
unsigned char bin_buf[128];
void Data_clear(unsigned char *data,unsigned long int len)
{
     unsigned long int i;
     for(i = 0;i < len;i++)
     {
         *data = 0;
         data++;
     }
}
unsigned char convertion(unsigned char *hex_data)
{
    unsigned char bin_data = 0xFF;
    if(*hex_data >= '0'&&*hex_data <= '9')
    {

        bin_data = *hex_data-NUM_OFFSET;
    }
    else if(*hex_data >= 'A'&&*hex_data <= 'F')
    {
        bin_data = *hex_data-CHAR_OFFSET;
    }
    return bin_data;
}
void hex_to_bin(unsigned char *hex_src,unsigned char *bin_dst,unsigned char len)
{
    unsigned char i;
    for(i = 0;i <len;i++)
    {
        *bin_dst = convertion(hex_src);
        bin_dst++;
        hex_src++;
    }
}
//__attribute__((section("ccm")))
u8 Hex_to_Bin(void)
{
	//After Build - User command #1: fromelf.exe --bin -o ..\output\hex_to_bin.bin ..\output\hex_to_bin.axf
	u8 i;//常用的数据变量无实际意义;
	res = f_mount(&fatfs,"0:",1);
	if(res != FR_OK)
	{
		return 1;//文件系统挂载失败
	}
	res = f_open(&fp_hex,"0:/hex_to_bin/hex_to_bin.hex",FA_READ);
	if(res != FR_OK)
	{
		printf("//打开28335SCI_Update_Flash.hex文件失败,");
		return 2;//打开28335SCI_Update_Flash.hex文件失败,
	}
	res = f_open(&fp_bin,"0:/hex_to_bin/28335SCI_Update_Flash.bin",FA_OPEN_ALWAYS);
	if(res != FR_OK)
	{
		printf("//打开28335SCI_Update_Flash.bin文件失败,");
		return 3;//打开28335SCI_Update_Flash.hex文件失败,
	}
	res  = f_lseek(&fp_hex,0);
	if(res != FR_OK)
	{
		printf("4");
		return 4; 
	}
	res  = f_lseek(&fp_bin,0);
	if(res != FR_OK)
	{
		printf("5");
		return 5; 
	}
	while(hex_size < f_size(&fp_hex))
	{
		res = f_read(&fp_hex,read_buf,9,&read_cnt);
		if(res != FR_OK)
		{
			printf("6");
			return 6; 
		}
		//------------------------------------
	    //---------------------END--------------------
		if(read_buf[0] == ':')//表示是起始标志
		{	
			hex_to_bin(&read_buf[1],bin_buf,8);  
			pack_info.data_len = bin_buf[0]<<4|bin_buf[1];
			pack_info.data_addr_offset = bin_buf[2]<<12|bin_buf[3]<<8|bin_buf[4]<<4|bin_buf[5];
			pack_info.data_type = bin_buf[6]<<4|bin_buf[7]; 
			#if USE_DEBUG
				for(i = 0;i < 8;i++)
				{ 
					//打印bin_buf 
					if(bin_buf[i] <0x10)
					{
						if(bin_buf[i] == 0)
						{
							printf("%X",bin_buf[i]);
						}
						else
						{
							printf("%X",bin_buf[i]);
						}
						
					}
					else
					{
						printf("%X",bin_buf[i]);
					}
				}
			#endif
			Data_clear(read_buf,128);
			Data_clear(bin_buf,128);
		//  fread(read_buf,(pack_info.data_len*2+3),1,fp);
			res = f_read(&fp_hex,read_buf,(pack_info.data_len*2+3),&read_cnt);//第二次读取文件
			if(res != FR_OK)
			{
				printf("7");
				return 7; 
			}
			hex_to_bin(&read_buf[0],bin_buf,pack_info.data_len*2);
			if(pack_info.data_type == 0x04)
			{
				pack_info.data_addr =bin_buf[0]<<12|bin_buf[1]<<8|bin_buf[2]<<4|bin_buf[3];
				pack_info.data_addr = pack_info.data_addr<<16;
			}
			 else if(pack_info.data_type == 0x00)
			{
				pack_info.data_addr = pack_info.data_addr+pack_info.data_addr;//表示该行数据应该写入的真实地址
				for(i = 0;i <pack_info.data_len;i++)
				{
				   pack_info.Data[i] = bin_buf[i*2]<<4|bin_buf[2*i+1];
				}
				//以下代码还需进一步进行测试,年前应该没时间了,明天需要做摇摇棒相关工作
				//fwrite(&pack_info.Data[0],pack_info.data_len,1,fp_bin);
				/*
				res = f_write(&fp_bin,&pack_info.Data[0],pack_info.data_len,&write_cnt);
				if(res != FR_OK)
				{
					printf("8");
					return 8; 
				}*/
				#if USE_DEBUG 
					for(i = 0;i < pack_info.data_len*2;i++)
					{ 
						//打印bin_buf
						if(bin_buf[i] <0x10)
						{
							if(bin_buf[i] == 0)
							{
								printf("%X",bin_buf[i]);
							}
							else
							{
								printf("%X",bin_buf[i]);
							}
						}
						else
						{
							printf("%X",bin_buf[i]);
						}
					}	
				#endif
			}
			Data_clear(read_buf,128);
			Data_clear(bin_buf,128);
			Data_clear(bin_buf,128);
			f_lseek(&fp_hex,(f_tell(&fp_hex)+1));
		}
		hex_size =hex_size+(pack_info.data_len*2+12);
		printf("\r\n");
	}
	f_close(&fp_bin);
	f_close(&fp_hex); 
	return 0xFF;

}
