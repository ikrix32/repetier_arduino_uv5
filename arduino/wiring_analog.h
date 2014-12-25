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

#ifndef _WIRING_ANALOG_
#define _WIRING_ANALOG_

#ifdef __cplusplus
extern "C" {
#endif

//typedef void (*myfunc)(uint32_t);

#define ADC_RESOLUTION        10
#define PWM_RESOLUTION        16

#define WRITE_CURRENT_RESOLUTION    8
#define READ_CURRENT_RESOLUTION		10

//extern void uartregist(myfunc handle);

extern void analogWrite( uint32_t ulPin, uint32_t ulValue );
extern uint32_t analogRead( uint32_t ulPin );

extern void analogReference( uint32_t type );
extern void analogInpselType( uint32_t type);
//extern void analogExtReference( uint32_t type );
extern void analogReadResolution( uint8_t resolution);
extern void analogWriteResolution( uint8_t resolution );

extern uint8_t GPIOTE_Channel_Find();
extern void GPIOTE_Channel_Set(uint8_t channel);
extern void GPIOTE_Channel_Clean(uint8_t channel);

extern int find_free_PPI_channel(int exclude_channel);

void PPI_Off_FROM_GPIO(uint32_t pin);

#ifdef __cplusplus
}
#endif

#endif 
