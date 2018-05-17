#include "Fun_41.h"
#include "W25Q64BV.h"
/* ----------------------- Static functions ---------------------------------*/
eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );
/************************************************************** 
* 0x41 д�������ּĴ���,������SPI_FLASH
* 0x42 д����Ĵ���������SPI_FLASH
* 0x43 ��SPI_FLASH��ȡ����
****************************************************************/
#if FUN_READ_HOLDING_ENABLED > 0  //0x43 ��SPI_FLASH��ȡ����
eMBException Fun_ReadHoldingRegister( UCHAR * pucFrame, USHORT * usLen )
{
	  USHORT          usRegAddress;
    USHORT          usRegCount;
    UCHAR          *pucFrameCur;
    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;
    if( *usLen == ( Fun_READ_SIZE + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress  = ( USHORT )( pucFrame[Fun_READ_ADDR_OFF] << 8 );
        usRegAddress |= ( USHORT )( pucFrame[Fun_READ_ADDR_OFF + 1] ); 
        usRegCount = ( USHORT )( pucFrame[Fun_READ_REGCNT_OFF] << 8 );
        usRegCount = ( USHORT )( pucFrame[Fun_READ_REGCNT_OFF + 1] );
        if( ( usRegCount >= 1 ) && ( usRegCount <= Fun_READ_REGCNT_MAX ) )
        { 
            pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
            *usLen = MB_PDU_FUNC_OFF;  
            *pucFrameCur++ = 0x43;
            *usLen += 1;  
            *pucFrameCur++ = ( UCHAR ) ( usRegCount * 2 );
            *usLen += 1;  
             eRegStatus = Fun_RegHolding( pucFrameCur, usRegAddress, usRegCount, MB_REG_READ ); 
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBError2Exception( eRegStatus );
            }
            else
            {
                *usLen += usRegCount * 2;
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    { 
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}
#endif

#if FUN_WRITE_HOLDING_ENABLED > 0 //0x41 д�������ּĴ���,������SPI_FLASH
eMBException Fun_WriteHoldingRegister( UCHAR * pucFrame, USHORT * usLen )
{
		 USHORT          usRegAddress;
    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus; 
    if( *usLen == ( Fun_WRITE_SIZE + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress = ( USHORT )( pucFrame[Fun_WRITE_ADDR_OFF] << 8 );
        usRegAddress |= ( USHORT )( pucFrame[Fun_WRITE_ADDR_OFF + 1] ); 
        eRegStatus = Fun_RegHolding( &pucFrame[Fun_WRITE_VALUE_OFF], usRegAddress, 1, MB_REG_WRITE ); 
        if( eRegStatus != MB_ENOERR )
        {
            eStatus = prveMBError2Exception( eRegStatus );
        }
    }
    else
    { 
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}
#endif

#if FUN_WRITE_MULTIPLE_HOLDING_ENABLED > 0 // 0x42 д����Ĵ���������SPI_FLASH
eMBException Fun_WriteMultipleHoldingRegister( UCHAR * pucFrame, USHORT * usLen )
{
 USHORT          usRegAddress;
    USHORT          usRegCount;
    UCHAR           ucRegByteCount;

    eMBException    eStatus = MB_EX_NONE;
    eMBErrorCode    eRegStatus;

    if( *usLen >= ( Fun_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN ) )
    {
        usRegAddress = ( USHORT )( pucFrame[Fun_WRITE_MUL_ADDR_OFF] << 8 );
        usRegAddress |= ( USHORT )( pucFrame[Fun_WRITE_MUL_ADDR_OFF + 1] );
        usRegCount = ( USHORT )( pucFrame[Fun_WRITE_MUL_REGCNT_OFF] << 8 );
        usRegCount |= ( USHORT )( pucFrame[Fun_WRITE_MUL_REGCNT_OFF + 1] );
        ucRegByteCount = pucFrame[Fun_WRITE_MUL_BYTECNT_OFF];
        if( ( usRegCount >= 1 ) &&
            ( usRegCount <= Fun_WRITE_MUL_REGCNT_MAX ) &&
            ( ucRegByteCount == ( UCHAR ) ( 2 * usRegCount ) ) )
        { 
            eRegStatus = Fun_RegHolding( &pucFrame[Fun_WRITE_MUL_VALUES_OFF], usRegAddress, usRegCount, MB_REG_WRITE ); 
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBError2Exception( eRegStatus );
            }
            else
            { 
                *usLen = Fun_WRITE_MUL_BYTECNT_OFF;
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
    else
    { 
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
    return eStatus;
}
#endif 
/**
  *****************************************************************************
  * @Name   : �������ּĴ���
  *
  * @Brief  : ��Ӧ������  0x41 д�������ּĴ���,������SPI_FLASH
  * 											0x42 д����Ĵ���������SPI_FLASH
  *												0x43 ��SPI_FLASH��ȡ����
  *
  * @Input  : *pucRegBuffer�����ݻ���������Ӧ������
  *           usAddress:     �Ĵ�����ַ
  *           usNRegs:       �����Ĵ�������
  *           eMode:         ������
  *
  * @Output : none
  *
  * @Return : Modbus״̬��Ϣ
  *****************************************************************************
**/
static eMBErrorCode  Fun_RegHolding( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode eStatus = MB_ENOERR; 
	//�жϵ�ַ�Ƿ�Ϸ� 
	if((usAddress >= REG_HOLDING_START) && ((usAddress + usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		switch(eMode)
		{
			case MB_REG_READ:  //�����ּĴ��� 	
					Read_Buffer(usAddress,usNRegs*2,pucRegBuffer);
					break; 
			case MB_REG_WRITE:  //д���ּĴ��� 
			SPI_Flash_WR(usAddress,usNRegs*2,pucRegBuffer);
					break;
		}
	}
	else  //�����ַ
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}
