/*
  Copyright (c) 2011-2012 Arduino.  All right reserved.

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

#ifndef _WIRING_INTERRUPTS_
#define _WIRING_INTERRUPTS_

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DYNAMIC_HANDLERS

typedef void (*dynamic_handler_t)(void);

extern void attachInterrupt(uint8_t pin, dynamic_handler_t event_handler, uint32_t mode);
extern void detachInterrupt(uint32_t pin );

extern void LinkInterrupt( uint8_t IRQn, dynamic_handler_t handler);
extern void UnlinkInterrupt( uint8_t IRQn );
#endif
	
#ifdef __cplusplus
}
#endif

#endif /* _WIRING_INTERRUPTS_ */
