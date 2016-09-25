
# FreqMeasure

This Arduino library uses the mega328 Timer1 hardware to measure the period of a waveform.  The capture interrupt is used
to record the timer value on the leading edges of the input signal, and the overflow interrupt is tracked so that periods
greater than 64k clock cycles can be measured.

Multiple periods are gathered into a small buffer, and you can read back either the latest available period or the
mean period of the last few periods.

Because the input-capture hardware is used, the input signal must be connected to ICP1/PB0/pin-8.

This library can probably not be used concurrently with any other that uses the Timer1 hardware.
