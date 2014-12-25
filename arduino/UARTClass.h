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

#ifndef _UART_CLASS_
#define _UART_CLASS_

#include "HardwareSerial.h"
#include "RingBuffer.h"

typedef void (*uart_callback_t)(void);

class UARTClass : public HardwareSerial
{
  protected:
    RingBuffer*	rx_buffer ;
		uart_callback_t    UART_CallBack;
  public:
		UARTClass(RingBuffer * pRx_buffer );

	void begin( const uint32_t dwBaudRate ) ;
	void begin(const uint32_t dwBaudRate, uint32_t rx_pin, uint32_t tx_pin);
	void end( void ) ;
		
	virtual int available( void ) ;
	virtual int peek( void ) ;	
  virtual int read( void ) ;
	virtual void flush( void ) ;
	virtual size_t write( const uint8_t c ) ;
		
	void IrqHandler( void ) ;		
	void irq_attach( uart_callback_t UART_CallBackFunc );
        
#if defined __GNUC__ /* GCC CS3 */
    //using Print::write ; // pull in write(str) and write(buf, size) from Print
#elif defined __ICCARM__ /* IAR Ewarm 5.41+ */
//    virtual void write( const char *str ) ;
//    virtual void write( const uint8_t *buffer, size_t size ) ;
#endif

		virtual operator bool() { return true; }; // USART always active
};

#endif // _UART_CLASS_
