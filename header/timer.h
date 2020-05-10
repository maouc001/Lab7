#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. c programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model
unsigned long _avr_timer_M = 1;		// Start count from here, down to 0. Default 1 ms
unsigned long _avr_timer_cntcurr = 0;	// Current internal count of 1 ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B; 	// bit3 = 0: CTC mode (clear timer on compare)
			// bit2 bit1 bit0 = 011: pre-scaler/64
			// 0000 1011: 0x0B
			// SO, 8 Mhz clock or 8,000,000/ 64 = 125,000 ticks/s
			// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCRIA
	OCR1A = 125; 	// Timer interrupt will be generated when TCNT1 == OCR1A
			// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
			// So when TCNT1 register equals 125
	
	// AVR timer interrupt mask register
	TIMSK1 = 0x02;	// bit1: OCIE1A -- enables compare match interrupt

	// Initialize avr counter 
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	// timerISR will be called every _avr_timer_cntcurr milliseconds

	// Enable global interrupts 
	SREG |= 0X80;	// 0X80:1000 0000 
} 

void TimerISR() {
	TimerFlag = 1;
}

// in our approach, the c programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn setting)
	_avr_timer_cntcurr --;			// count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { 		// results in a more efficient compare
		TimerISR();			// call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

