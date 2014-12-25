/*
    This file is part of Repetier-Firmware.

    Repetier-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Repetier-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.

    This firmware is a nearly complete rewrite of the sprinter firmware
    by kliment (https://github.com/kliment/Sprinter)
    which based on Tonokip RepRap firmware rewrite based off of Hydra-mmm firmware.
*/

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

class Console
{
	public:
			static void printNumber(uint32_t n);
			static void printWarningF(FSTRINGPARAM(text));
			static void printInfoF(FSTRINGPARAM(text));
			static void printErrorF(FSTRINGPARAM(text));
			static void printWarningFLN(FSTRINGPARAM(text));
			static void printInfoFLN(FSTRINGPARAM(text));
			static void printErrorFLN(FSTRINGPARAM(text));
			static void printFLN(FSTRINGPARAM(text));
			static void printF(FSTRINGPARAM(text));
			static void printF(FSTRINGPARAM(text),int value);
			static void printF(FSTRINGPARAM(text),const char *msg);
			//krix static void printF(FSTRINGPARAM(text),int32_t value);
			//krix static void printF(FSTRINGPARAM(text),uint32_t value);
			static void printF(FSTRINGPARAM(text),float value,uint8_t digits);
			static void printFLN(FSTRINGPARAM(text),int value);
			//krix static void printFLN(FSTRINGPARAM(text),int32_t value);
			//krix static void printFLN(FSTRINGPARAM(text),uint32_t value);
			static void printFLN(FSTRINGPARAM(text),const char *msg);
			static void printFLN(FSTRINGPARAM(text),float value,uint8_t digits);
			static void printArrayFLN(FSTRINGPARAM(text),float *arr,uint8_t n=4,uint8_t digits=2);
			static void printArrayFLN(FSTRINGPARAM(text),int32_t *arr,uint8_t n=4);
			static void print(long value);
			static inline void print(uint32_t value) {printNumber(value);}
			static inline void print(int value) {print((int32_t)value);}
			static void print(const char *text);
			static inline void print(char c) {HAL::serialWriteByte(c);}
			static void printFloat(float number, uint8_t digits);
			static inline void print(float number) {printFloat(number, 6);}
			static inline void println() {HAL::serialWriteByte('\r');HAL::serialWriteByte('\n');}
    protected:
    private:
};

#ifdef DEBUG
#define SHOW(x) {Console::printF(PSTR(" " #x "=")); Console::print(x); Console::println();}
#define SHOWS(x) {Console::printF(PSTR(" " #x "=")); Console::print(x); Console::print(" steps  "); Console::print(x/80); Console::printFLN(PSTR(" mm"));}
#define SHOWM(x) {Console::printF(PSTR(" " #x "=")); Console::print((long)x*80); Console::print(" steps  "); Console::print(x); Console::printFLN(PSTR(" mm"));}
#define SHOT(x) Console::printF(PSTR(x " "))
#define SHOWA(t,a,n) {SHOT(t); for (int i=0;i<n;i++) SHOWS(a[i]);}
#define SHOWAM(t,a,n) {SHOT(t); for (int i=0;i<n;i++) SHOWM(a[i]);}

#else
#define SHOW(x)
#define SHOT(x)
#define SHOWS(x)
#define SHOWM(x)
#define SHOWA(t,a,n)
#define SHOWAM(t,a,n)
#endif

#endif // COMMUNICATION_H
