#ifndef _PORT_H
#define _PORT_H
#include <assert.h>
#include <inttypes.h>
#include "stm32f4xx.h"
//#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }
void EnterCriticalSection(void);//�ر�ȫ���ж�
void ExitCriticalSection(void);//��ȫ���ж�
#define ENTER_CRITICAL_SECTION( )   __disable_irq()//__set_PRIMASK(1)  //�����ж�  __disable_irq();
#define EXIT_CRITICAL_SECTION( )    __enable_irq()//__set_PRIMASK(0)  //�����ж�

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
                               ����Modbus��ز���
******************************************************************************/
//����Ĵ�����ز���
#define REG_INPUT_START                          (USHORT)0x0001  //��ʼ�Ĵ���
#define REG_INPUT_NREGS                          (USHORT)40  //�Ĵ�������
//���ּĴ�����ز���
#define REG_HOLDING_START                        (USHORT)50  //���ּĴ���
#define REG_HOLDING_NREGS                        (USHORT)20  //���ּĴ�������
//��Ȧ�Ĵ�����ز���
#define REG_Coils_START                          (USHORT)80//��Ȧ�Ĵ�����ʼ��ַ
#define REG_Coils_NREGS                          (USHORT)24//��Ȧ�Ĵ�������(�����8�ı���;
//��ɢ�Ĵ�����ز���
#define REG_Discrete_START						           (USHORT)120
#define REG_Discrete_NREG						             (USHORT)32
extern USHORT usRegInputBuf[REG_INPUT_NREGS];//����Ĵ���,modbusЭ��ֻ�ܶ�ȡ�üĴ���,�����޸�
extern USHORT usRegHoldingStart;
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];//���ּĴ���,modbusЭ������޸ĸüĴ���,���ܶ�ȡ�üĴ���
extern UCHAR CoilsReg_Buf[3];//��Ȧ�Ĵ������飬�����鳤��Ϊ��Ȧ�Ĵ����ĸ�������8,���������Ϊ0,��1,�üĴ���Э��ɶ���д
extern UCHAR DiscreteReg_Buf[4];
#endif
