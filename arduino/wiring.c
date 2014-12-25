/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif
	
#define APP_TIMER_PRESCALER             0                       /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            (2 + 2)                 /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                       /**< Size of timer operation queues. */

#define RTC1_CYCLE_TIME                      APP_TIMER_TICKS(120000, 0)

static app_timer_id_t                        m_rtc_start_timer_id; 

// widen rtc1 to 40-bit (1099511627775 ticks = 33554431999969us = 388 days)
// (dont overflow uint64_t when multipying by 1000000)
static uint64_t rtc1_owerflow_count=0;
//volatile uint64_t rtc1 = 0;
//static uint32_t trc1_cc_value=0, count=0;
/**********************************************************************
name :
function : 
**********************************************************************/

/**********************************************************************
name :
function : 
**********************************************************************/
/*
//void RTC1_IRQHandler()
static void rtc1_handler()
{
	if (NRF_RTC1->EVENTS_OVRFLW)
	{
		// extend counter
		rtc1 += 0x1000000LLU;
		// truncate to 40-bit
		rtc1 &= 0xffffffffffLLU;
		NRF_RTC1->EVENTS_OVRFLW = 0;
	}
	if(NRF_RTC1->EVENTS_COMPARE[0] != 0 )
	{	//about 30.517 * 33 = 1007.061
		count++;
		if(count >= 1000)
		{	
			count = 0;
			nrf_gpio_pin_toggle(15);
		}
		
		trc1_cc_value = NRF_RTC1->COUNTER + 33;
		NRF_RTC1->CC[0] = trc1_cc_value & 0x00FFFFFF;
		
		NRF_RTC1->EVENTS_COMPARE[0] = 0;
	}
}
*/
/**********************************************************************
name :
function : 
**********************************************************************/
/*
void delay( uint32_t ms )
{	
    //NVIC_EnableIRQ(RTC0_IRQn);      
	// Enable Interrupt for the RTC in the core.
    NRF_RTC0->PRESCALER     = 32;                    // Set prescaler to a TICK of RTC_FREQUENCY.
    NRF_RTC0->CC[0]         = ms;  // Compare0 after approx COMPARE_COUNTERTIME seconds.

    // Enable TICK event and TICK interrupt:
    // NRF_RTC0->EVTENSET      = RTC_EVTENSET_TICK_Msk;
    //NRF_RTC0->INTENSET      = RTC_INTENSET_TICK_Msk;

    // Enable COMPARE0 event and COMPARE0 interrupt:
    NRF_RTC0->EVTENSET      = RTC_EVTENSET_COMPARE0_Msk;
    NRF_RTC0->INTENSET      = RTC_INTENSET_COMPARE0_Msk;
	NRF_RTC0->TASKS_CLEAR = 1;
	
	NRF_RTC0->TASKS_START = 1;	
	while(NRF_RTC0->EVENTS_COMPARE[0] == 0);
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
	NRF_RTC0->TASKS_STOP = 1;
}
*/
/**********************************************************************
name :
function : 
**********************************************************************/
void timer_error_check(uint32_t err_code)
{
	if(err_code != NRF_SUCCESS)
	{
		NVIC_SystemReset();
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void rtc_timout_timer_handle(void * p_context)
{
	//UNUSED_PARAMETER(p_context);
}
/**********************************************************************
name :
function : 
**********************************************************************/
void rtc_timer_init()
{	
	uint32_t err_code;
	
	APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);  
	
	err_code = app_timer_create(&m_rtc_start_timer_id,
					 APP_TIMER_MODE_REPEATED,
					 rtc_timout_timer_handle);
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_start(m_rtc_start_timer_id, RTC1_CYCLE_TIME, NULL);
  APP_ERROR_CHECK(err_code);	
	
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint64_t millis64( void )
{	
	//divide by 32768
	uint32_t time_;
	app_timer_cnt_get( &time_ );
	return (rtc1_owerflow_count + time_) * 1000 >> 15;  
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint64_t micros64( void )
{
	//accurate to 30.517us, divide by 32768
	uint32_t time_;
	app_timer_cnt_get( &time_ );
	return (rtc1_owerflow_count + time_) * 1000000 >> 15;  
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint32_t millis( void )
{
	return millis64();
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint32_t micros( void )
{
	return micros64();
}
/**********************************************************************
name :
function : 
**********************************************************************/
void delay( uint32_t ms )
{
	/*uint32_t start = millis();
	while (millis() - start < ms)
	{
	    yield();
	}*/
	nrf_delay_ms(ms);
}
/**********************************************************************
name :
function : 
**********************************************************************/
void delayMicroseconds( uint32_t us )
{
   nrf_delay_us(us);
}

#ifdef __cplusplus
}
#endif
