/* -*- C++ -*- */
/*
    Copyright (C) 2015 William Brodie-Tyrrell
    william@brodie-tyrrell.org
  
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of   
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef _FREQMEASURE_H_
#define _FREQMEASURE_H_

#include <Arduino.h>

#define FREQPIN 5
#define PERIODPIN 8

#define T1_PRESCALE_1 0x01
#define T1_PRESCALE_8 0x02
#define T1_PRESCALE_64 0x03
#define T1_PRESCALE_256 0x04
#define T1_PRESCALE_1024 0x05

extern void (*T1_overflow)();
extern void (*T1_capture)();

#define PERIOD_WINDOW_BITS 3
#define MAXACCUM 32

/**
 * Class to measure period on an external pin by connecting it to Timer1.
 * Expect signal to be on Pin 8 (PB0/ICP1)
 */
// template<int PERIOD_WINDOW_BITS, int MAXACCUM>
class PeriodMeasure {
public:

    /// configure and begin measurements
    static void setup(char prescale = T1_PRESCALE_1);

    /// turn off the interrupts that are doing the measurements
    static void stop();

    /// read latest smoothed value of the period
    static long readSmoothed();

    /// read latest period value; no smoothing
    static long readLatest();

    /// how many periods have we seen
    static char getPeriodCount();

private:

    static void interrupt_overflow();

    static void interrupt_capture();

    // number of timer overflows
    static short accumPeriod;
    static long periodWindow[1<<PERIOD_WINDOW_BITS];
    static char periodWindowPtr;
};



#endif // _FREQMEASURE_H_
