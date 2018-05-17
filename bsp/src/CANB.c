/*******************************************************
 * file_name  : CANB.c
 * DIR:/28335_APP/bsp/src/CANB.c
 * Created on : 2017年11月15日-下午8:25:38
 * Author     : admin
 * Version    :
 *
 ******************************************************/
#include "CANB.h"
tag_mbxCfg CANB_tx_msg[12];
u8 dir = 0;
unsigned short int speed_fbk = 0;
unsigned short int speed_ref = 0;
void CAN_tx_msg_init(void)
{
	int i;
	unsigned long int ID = 0x4185;
	for(i = 0;i<12;i++)
	{
		CANB_tx_msg[i].Enable = 1;
		CANB_tx_msg[i].cnt = 0;
		CANB_tx_msg[i].flag = 0;
		CANB_tx_msg[i].txEnable = 1; 
		CANB_tx_msg[i].can_tx_msg.DLC = 8;
		CANB_tx_msg[i].can_tx_msg.IDE =CAN_ID_EXT; 
		CANB_tx_msg[i].can_tx_msg.ExtId = ID+i; 

	}
	CANB_tx_msg[0].prd  =10;
	CANB_tx_msg[1].prd  =20;
	CANB_tx_msg[2].prd  =20;
	CANB_tx_msg[3].prd  =20;
	CANB_tx_msg[4].prd  =10;
	CANB_tx_msg[5].prd  =10;
	CANB_tx_msg[6].prd  =20;
	CANB_tx_msg[7].prd  =20;
	CANB_tx_msg[8].prd  =20;
	CANB_tx_msg[9].prd  =20;
	CANB_tx_msg[10].prd =20;
	CANB_tx_msg[11].prd =20;
}

void packcanbMbx0(void)//0x4185
{
	CANB_tx_msg[0].can_tx_msg.ExtId  = 0x4185;
	int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[0].can_tx_msg.Data[i] = 0x15+i;
		
	} 
}
void packcanbMbx1(void)//0x4186
{
	CANB_tx_msg[1].can_tx_msg.ExtId  = 0x4186;
		int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[1].can_tx_msg.Data[i] = 0x26+i;
		
	}  
}
void packcanbMbx2(void)//0x4187
{
	CANB_tx_msg[2].can_tx_msg.ExtId = 0x4187;
	int i;
	for(i = 0;i<4;i++)
	{
		CANB_tx_msg[2].can_tx_msg.Data[i] = 0x37+i;
		
	} 
	CANB_tx_msg[2].can_tx_msg.Data[4] = (speed_ref)&0xFF;;
	CANB_tx_msg[2].can_tx_msg.Data[5] = (speed_ref>>8)&0xFF;
	CANB_tx_msg[2].can_tx_msg.Data[6] = (speed_fbk)&0xFF;
	CANB_tx_msg[2].can_tx_msg.Data[7] = (speed_fbk>>8)&0xFF;
}
void packcanbMbx3(void)//0x4188
{
	CANB_tx_msg[3].can_tx_msg.ExtId = 0x4188;
		int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[3].can_tx_msg.Data[i] = 0x48+i;
		
	} 
}
void packcanbMbx4(void)//0x4189
{
	CANB_tx_msg[4].can_tx_msg.ExtId   = 0x4189;
	int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[4].can_tx_msg.Data[i] = 0x59+i;
		
	} 
}
void packcanbMbx5(void)//0x418A
{
	CANB_tx_msg[5].can_tx_msg.ExtId   = 0x418A;
	int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[5].can_tx_msg.Data[i] = 0x6A+i; 
	}  
}
void packcanbMbx6(void)//0x418B
{
	CANB_tx_msg[6].can_tx_msg.ExtId   = 0x418B;
	int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[6].can_tx_msg.Data[i] = 0x7B+i;
		
	}  
}
void packcanbMbx7(void)//0x418C
{
	CANB_tx_msg[7].can_tx_msg.ExtId   = 0x418C;
	int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[7].can_tx_msg.Data[i] = 0x8C+i;
		
	}  
}
void packcanbMbx8(void)//0x418D
{
	CANB_tx_msg[8].can_tx_msg.ExtId   = 0x418D;
	int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[8].can_tx_msg.Data[i] = 0x9D+i;
		
	}  
}
void packcanbMbx9(void)//0x418E
{
	CANB_tx_msg[9].can_tx_msg.ExtId   = 0x418E;
	int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[9].can_tx_msg.Data[i] = 0xAE+i;
		
	}  
}
void packcanbMbx10(void)//0x418F
{
	CANB_tx_msg[10].can_tx_msg.ExtId   = 0x418F;
	int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[10].can_tx_msg.Data[i] = 0xBA+i;
		
	}  
}
void packcanbMbx11(void)//0x4190
{
	CANB_tx_msg[11].can_tx_msg.ExtId   = 0x4190;
	int i;
	for(i = 0;i<8;i++)
	{
		CANB_tx_msg[11].can_tx_msg.Data[i] = 0xC0+i;
		
	}   
}
void canbMbxSlotCtrl(void) 
{
    int i;
    if(dir == 0)
    {
    	speed_fbk++;
    	speed_ref  = 0x4190-speed_fbk;
    	if(speed_fbk > 0x4190)
    	{
    		dir = 1;
    	}
    }
    else if(dir == 1)
    {
    	if(speed_fbk >5)
    	{
    		speed_fbk--;
    		speed_ref = 0x4190-speed_fbk;
    	}
    	else
    	{
    		dir = 0;
    	}
    }
	for(i=0;i<12;i++)
	{
		CANB_tx_msg[i].cnt++;
		if(CANB_tx_msg[i].cnt>=CANB_tx_msg[i].prd)
		{
			CANB_tx_msg[i].flag=1;
			CANB_tx_msg[i].cnt=0;
		}
	}
}
void canbtransmit(void)
{
	canbMbxSlotCtrl();
	if((CANB_tx_msg[0].txEnable) && (CANB_tx_msg[0].flag) &&CANB_tx_msg[0].Enable)//4185
	{
		packcanbMbx0();
		CAN_WriteData(&CANB_tx_msg[0].can_tx_msg);
		CANB_tx_msg[0].flag=0;
	}

	if((CANB_tx_msg[1].txEnable) && (CANB_tx_msg[1].flag)&&CANB_tx_msg[1].Enable)//4186
	{
		packcanbMbx1();
		CAN_WriteData(&CANB_tx_msg[1].can_tx_msg);
		CANB_tx_msg[1].flag=0;
	}
	if((CANB_tx_msg[2].txEnable) && (CANB_tx_msg[2].flag)&&CANB_tx_msg[2].Enable)//4187
	{
		packcanbMbx2();
		CAN_WriteData(&CANB_tx_msg[2].can_tx_msg);
		CANB_tx_msg[2].flag=0;
	}

	if((CANB_tx_msg[3].txEnable) && (CANB_tx_msg[3].flag)&&CANB_tx_msg[3].Enable)//4188
	{
		packcanbMbx3();
		CAN_WriteData(&CANB_tx_msg[3].can_tx_msg); 
		CANB_tx_msg[3].flag=0;
	}
	if((CANB_tx_msg[4].txEnable) && (CANB_tx_msg[4].flag)&&CANB_tx_msg[4].Enable)//4189
	{
		packcanbMbx4();
		CAN_WriteData(&CANB_tx_msg[4].can_tx_msg); 
		CANB_tx_msg[4].flag=0;
	}

	if((CANB_tx_msg[5].txEnable) && (CANB_tx_msg[5].flag)&&CANB_tx_msg[5].Enable)//418A
	{
		packcanbMbx5();
		CAN_WriteData(&CANB_tx_msg[5].can_tx_msg); 
		CANB_tx_msg[5].flag=0;
	}
	if((CANB_tx_msg[6].txEnable) && (CANB_tx_msg[6].flag)&&CANB_tx_msg[6].Enable)//418B
	{
		packcanbMbx6();
		CAN_WriteData(&CANB_tx_msg[6].can_tx_msg); 
		CANB_tx_msg[6].flag=0;
	}
	if((CANB_tx_msg[7].txEnable) && (CANB_tx_msg[7].flag)&&CANB_tx_msg[7].Enable)//418C
	{
		packcanbMbx7();
		CAN_WriteData(&CANB_tx_msg[7].can_tx_msg); 
		CANB_tx_msg[7].flag=0;
	}
	if((CANB_tx_msg[8].txEnable) && (CANB_tx_msg[8].flag)&&CANB_tx_msg[8].Enable)//418D
	{
		packcanbMbx8();
		CAN_WriteData(&CANB_tx_msg[8].can_tx_msg); 
		CANB_tx_msg[8].flag=0;
	}
	if((CANB_tx_msg[9].txEnable) && (CANB_tx_msg[9].flag)&&CANB_tx_msg[9].Enable)//418E
	{
		packcanbMbx9();
		CAN_WriteData(&CANB_tx_msg[9].can_tx_msg); 
		CANB_tx_msg[9].flag=0;
	}
	if((CANB_tx_msg[10].txEnable) && (CANB_tx_msg[10].flag)&&CANB_tx_msg[10].Enable)//418F
	{
		packcanbMbx10();
		CAN_WriteData(&CANB_tx_msg[10].can_tx_msg); 
		CANB_tx_msg[10].flag=0;
	}
	if((CANB_tx_msg[11].txEnable) && (CANB_tx_msg[11].flag)&&CANB_tx_msg[11].Enable)//4190
	{
		packcanbMbx11();
		CAN_WriteData(&CANB_tx_msg[11].can_tx_msg); 
		CANB_tx_msg[11].flag=0;
	}
}
