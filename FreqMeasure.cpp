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

#include <FreqMeasure.h>

void (*T1_overflow)() = NULL;
void (*T1_capture)() = NULL;

// template<int PERIOD_WINDOW_BITS, int MAXACCUM>
// short PeriodMeasure<PERIOD_WINDOW_BITS, MAXACCUM>::accumPeriod=0;
// template<int PERIOD_WINDOW_BITS, int MAXACCUM>
// long PeriodMeasure<PERIOD_WINDOW_BITS, MAXACCUM>::periodWindow[1<<PERIOD_WINDOW_BITS];
// template<int PERIOD_WINDOW_BITS, int MAXACCUM>
// char PeriodMeasure<PERIOD_WINDOW_BITS, MAXACCUM>::periodWindowPtr=-1;

short PeriodMeasure::accumPeriod;
long PeriodMeasure::periodWindow[1<<PERIOD_WINDOW_BITS];
char PeriodMeasure::periodWindowPtr;

/// fan rpm timer overflow
ISR(TIMER1_OVF_vect)
{
    (*T1_overflow)();
}

ISR(TIMER1_CAPT_vect)
{
    (*T1_capture)();
}

void PeriodMeasure::setup(char prescale)
{
    pinMode(FREQPIN, INPUT);
    pinMode(PERIODPIN, INPUT);
    digitalWrite(PERIODPIN, 1);  // weak pull-up

    for(char k=0;k<(1<<PERIOD_WINDOW_BITS);++k)
        periodWindow[k]=0;

    periodWindowPtr=-1;

    // period measurement (input capture)
    noInterrupts();
    T1_overflow=&interrupt_overflow;
    T1_capture=&interrupt_capture;
    TCCR1A=0x00;    // normal running timer
    TCCR1B=0xC0 | prescale;    // noise cancel, rising edge, selected-prescaler
    TIMSK1=0x21;    // input capture and overflow interrupts
    interrupts();
}

// turn off the interrupts that are doing the measurements
void PeriodMeasure::stop()
{
    TIMSK1=0x00;
}

/// read latest smoothed value of the period
long PeriodMeasure::readSmoothed()
{
    long sum=0;
    char count=1<<PERIOD_WINDOW_BITS;
    for(char k=0;k<count;++k)
        sum+=periodWindow[k];

    return sum/count;
}

/// read latest period value; no smoothing
long PeriodMeasure::readLatest()
{
    return periodWindow[periodWindowPtr];
}

/// how many periods have we seen
char PeriodMeasure::getPeriodCount()
{
    return periodWindowPtr;
}

void PeriodMeasure::interrupt_overflow()
{
    ++accumPeriod;
}

void PeriodMeasure::interrupt_capture()
{
    TCNT1=0;  // clear timer

    // is not an invalidly long time, record it
    if(accumPeriod <= MAXACCUM){
        // current timer plus all previous overflows
        long lastPeriod=(long(accumPeriod)<<16)+ICR1;
        // stash the value in the buffer and move ptr along
        periodWindow[++periodWindowPtr & ((1<<PERIOD_WINDOW_BITS)-1)]=lastPeriod;
    }

    accumPeriod=0;  
}
