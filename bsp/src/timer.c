/*******************************************************
 * file_name  : timer.c
 * DIR:/28335_APP/bsp/src/timer.c
 * Created on : 2017年8月11日-下午10:21:59
 * Author     : admin
 * Version    :
 *
 ******************************************************/
#include "timer.h"
#include "os.h" 
extern u16 count;
void TIM6_init(void)
{
	/*
	* 时钟是84MHz的频率,计时10ms
	*/
	TIM_TimeBaseInitTypeDef Time_base_init;
	NVIC_InitTypeDef NVIC_init;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	Time_base_init.TIM_CounterMode = TIM_CounterMode_Up;
	Time_base_init.TIM_Prescaler   = 84-1;
	Time_base_init.TIM_Period = 100;
	TIM_TimeBaseInit(TIM6,&Time_base_init);
	TIM_ARRPreloadConfig(TIM6,ENABLE);
	TIM_Cmd(TIM6,ENABLE);
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); 
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	NVIC_init.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_init.NVIC_IRQChannelPreemptionPriority = 0x0D;
	NVIC_init.NVIC_IRQChannelSubPriority = 0;
	NVIC_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_init);
}
void TIM6_DAC_IRQHandler(void)
{
		taskclock();
	if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	 
	} 
}