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


uint32_t pulseIn( uint32_t pin, uint32_t state, uint32_t timeout )
{
	uint32_t width = 0, nrf_pin; 
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	if(nrf_pin < 31)
	{
		uint32_t numloops = 0;
		uint32_t maxloops = microsecondsToClockCycles(timeout) / 2;

		// wait for any previous pulse to end
		while (((NRF_GPIO->IN >> nrf_pin) & 1UL) == state)
			if (numloops++ == maxloops)
				return 0;

		// wait for the pulse to start
		while (((NRF_GPIO->IN >> nrf_pin) & 1UL) != state)
			if (numloops++ == maxloops)
				return 0;

		// wait for the pulse to stop
		while (((NRF_GPIO->IN >> nrf_pin) & 1UL) == state) 
		{
			if (numloops++ == maxloops)
				return 0;
			width++;
		}
		
		return clockCyclesToMicroseconds(width * 17 + 16);
	}
	else 
	{
		return 0;
	}
}
