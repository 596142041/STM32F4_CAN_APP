#ifndef _PORT_H
#define _PORT_H
#include <assert.h>
#include <inttypes.h>
#include "stm32f4xx.h"
//#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }
void EnterCriticalSection(void);//关闭全局中断
void ExitCriticalSection(void);//打开全局中断
#define ENTER_CRITICAL_SECTION( )   __disable_irq()//__set_PRIMASK(1)  //关总中断  __disable_irq();
#define EXIT_CRITICAL_SECTION( )    __enable_irq()//__set_PRIMASK(0)  //开总中断

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif
/******************************************************************************
                               定义Modbus相关参数
******************************************************************************/
//输入寄存器相关参数
#define REG_INPUT_START                          (USHORT)0x0001  //起始寄存器
#define REG_INPUT_NREGS                          (USHORT)40  //寄存器个数
//保持寄存器相关参数
#define REG_HOLDING_START                        (USHORT)50  //保持寄存器
#define REG_HOLDING_NREGS                        (USHORT)20  //保持寄存器个数
//线圈寄存器相关参数
#define REG_Coils_START                          (USHORT)80//线圈寄存器起始地址
#define REG_Coils_NREGS                          (USHORT)24//线圈寄存器个数(最好是8的倍数;
//离散寄存器相关参数
#define REG_Discrete_START						           (USHORT)120
#define REG_Discrete_NREG						             (USHORT)32
extern USHORT usRegInputBuf[REG_INPUT_NREGS];//输入寄存器,modbus协议只能读取该寄存器,不能修改
extern USHORT usRegHoldingStart;
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];//保持寄存器,modbus协议既能修改该寄存器,又能读取该寄存器
extern UCHAR CoilsReg_Buf[3];//线圈寄存器数组，该数组长度为线圈寄存器的个数除以8,如果余数不为0,加1,该寄存器协议可读可写
extern UCHAR DiscreteReg_Buf[4];
#endif
