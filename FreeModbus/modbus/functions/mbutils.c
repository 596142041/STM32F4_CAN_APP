/****************************************************************************************
*该文件内部的函数主要是在线圈操作中会使用
*文件名:mbutils.c
*原作者:
*修改:廖品然
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
函数名:void xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits, UCHAR ucValue )
输入参数:
ucByteBuf 位存储的缓冲区。必须是2个字节的倍数。
 No length checking is performed and if usBitOffset / 8 is greater than the size of the buffer memory contents is overwritten.
usBitOffset 位设置的起始地址，第一个位的偏移为0。
ucNBits  需要修改的位的数量。该值必须小于8。
ucValues 位的新值。在usBitOffset中的第一位的值是ucValues的最低有效位;
返回值:无;
函数示例:
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
*函数名：UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits )
*输入参数:
*ucByteBuf 存储位的缓冲字节
*usBitOffset 位的起始地址，第一个位的地址偏移量是0
*ucNBits 需要修改的位的数量。该值要小于8。
*调用示例:
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
