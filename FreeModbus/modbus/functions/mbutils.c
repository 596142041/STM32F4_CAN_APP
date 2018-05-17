/****************************************************************************************
*���ļ��ڲ��ĺ�����Ҫ������Ȧ�����л�ʹ��
*�ļ���:mbutils.c
*ԭ����:
*�޸�:��ƷȻ
******************************************************************************************/

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbproto.h"

/* ----------------------- Defines ------------------------------------------*/
#define BITS_UCHAR      8U

/* ----------------------- Start implementation -----------------------------*/
/****************************************************************************
������:void xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits, UCHAR ucValue )
�������:
ucByteBuf λ�洢�Ļ�������������2���ֽڵı�����
 No length checking is performed and if usBitOffset / 8 is greater than the size of the buffer memory contents is overwritten.
usBitOffset λ���õ���ʼ��ַ����һ��λ��ƫ��Ϊ0��
ucNBits  ��Ҫ�޸ĵ�λ����������ֵ����С��8��
ucValues λ����ֵ����usBitOffset�еĵ�һλ��ֵ��ucValues�������Чλ;
����ֵ:��;
����ʾ��:
 ucBits[2] = {0, 0};
// Set bit 4 to 1 (read: set 1 bit starting at bit offset 4 to value 1)
xMBUtilSetBits( ucBits, 4, 1, 1 );
// Set bit 7 to 1 and bit 8 to 0.
xMBUtilSetBits( ucBits, 7, 2, 0x01 );
// Set bits 8 - 11 to 0x05 and bits 12 - 15 to 0x0A;
xMBUtilSetBits( ucBits, 8, 8, 0x5A);
***************************************************************************************/
void xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits,
                UCHAR ucValue )
{
    USHORT          usWordBuf;
    USHORT          usMask;
    USHORT          usByteOffset;
    USHORT          usNPreBits;
    USHORT          usValue = ucValue;

    assert( ucNBits <= 8 );
    assert( ( size_t )BITS_UCHAR == sizeof( UCHAR ) * 8 );

    /* Calculate byte offset for first byte containing the bit values starting
     * at usBitOffset. */
    usByteOffset = ( USHORT )( ( usBitOffset ) / BITS_UCHAR );

    /* How many bits precede our bits to set. */
    usNPreBits = ( USHORT )( usBitOffset - usByteOffset * BITS_UCHAR );

    /* Move bit field into position over bits to set */
    usValue <<= usNPreBits;

    /* Prepare a mask for setting the new bits. */
    usMask = ( USHORT )( ( 1 << ( USHORT ) ucNBits ) - 1 );
    usMask <<= usBitOffset - usByteOffset * BITS_UCHAR;

    /* copy bits into temporary storage. */
    usWordBuf = ucByteBuf[usByteOffset];
    usWordBuf |= ucByteBuf[usByteOffset + 1] << BITS_UCHAR;

    /* Zero out bit field bits and then or value bits into them. */
    usWordBuf = ( USHORT )( ( usWordBuf & ( ~usMask ) ) | usValue );

    /* move bits back into storage */
    ucByteBuf[usByteOffset] = ( UCHAR )( usWordBuf & 0xFF );
    ucByteBuf[usByteOffset + 1] = ( UCHAR )( usWordBuf >> BITS_UCHAR );
}
/*************************************************************************************
*��������UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits )
*�������:
*ucByteBuf �洢λ�Ļ����ֽ�
*usBitOffset λ����ʼ��ַ����һ��λ�ĵ�ַƫ������0
*ucNBits ��Ҫ�޸ĵ�λ����������ֵҪС��8��
*����ʾ��:
*UCHAR ucBits[2] = {0, 0};
UCHAR ucResult;
 
// Extract the bits 3 - 10.
ucResult = xMBUtilGetBits( ucBits, 3, 8 );

*
**************************************************************************************/
UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits )
{
    USHORT          usWordBuf;
    USHORT          usMask;
    USHORT          usByteOffset;
    USHORT          usNPreBits;

    /* Calculate byte offset for first byte containing the bit values starting
     * at usBitOffset. */
    usByteOffset = ( USHORT )( ( usBitOffset ) / BITS_UCHAR );

    /* How many bits precede our bits to set. */
    usNPreBits = ( USHORT )( usBitOffset - usByteOffset * BITS_UCHAR );

    /* Prepare a mask for setting the new bits. */
    usMask = ( USHORT )( ( 1 << ( USHORT ) ucNBits ) - 1 );

    /* copy bits into temporary storage. */
    usWordBuf = ucByteBuf[usByteOffset];
    usWordBuf |= ucByteBuf[usByteOffset + 1] << BITS_UCHAR;

    /* throw away unneeded bits. */
    usWordBuf >>= usNPreBits;

    /* mask away bits above the requested bitfield. */
    usWordBuf &= usMask;

    return ( UCHAR ) usWordBuf;
}

eMBException
prveMBError2Exception( eMBErrorCode eErrorCode )
{
    eMBException    eStatus;

    switch ( eErrorCode )
    {
        case MB_ENOERR:
            eStatus = MB_EX_NONE;
            break;

        case MB_ENOREG:
            eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
            break;

        case MB_ETIMEDOUT:
            eStatus = MB_EX_SLAVE_BUSY;
            break;

        default:
            eStatus = MB_EX_SLAVE_DEVICE_FAILURE;
            break;
    }

    return eStatus;
}
