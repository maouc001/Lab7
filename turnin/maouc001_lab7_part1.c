/*      Author: Mahdi Aouchiche
 *      Partner(s) Name: 
 *      Lab Section: 22
 *      Assignment: Lab # 7;  Exercise #1
 *      Exercise Description: [optional - include for your own benefit]
 *
 *      I acknowledge all content contained herein, excluding template or example
 *      code, is my own original work.
 */

#include <avr/io.h>
#include "io.h"
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum State {Start,S0, reset, increment, incrementWait, decrement, decrementWait, S9} state;

unsigned char cnt;	// holds the count to output to PORTC
unsigned char d;	// decrement d = ~PINA & 0x02
unsigned char i;	// increment i = ~PINA & 0x01
unsigned char t =0;	// used to increment or decrement each second

void counterTick() {
	// transitions
	switch (state) {
		case Start:
			state = S0;
			cnt = 0x00;
			break;
		case S0:
			if(i && ~d) {
				state = increment;
				cnt++;
				t = 0;
			} else {
				state = S0;
			}
			break;

		case reset:
                        if(i || d) {
                                state = reset;
                        } else {
				state = S0;
			} 
                        break;

		case increment:
			if( i && d) {
				state = reset;
			} else if(i && ~d){
				state = increment;
				t++;
				if(t >= 10) {
					t = 0;
					if(cnt < 0x09) {
						cnt++;
					} else {
						state = S9;
					}
				}
			} else { 
				state = incrementWait;
			}
			break;

		case incrementWait: 
			if (i && d) {
				state = reset;
			} else if(i && ~d && (cnt >=0x08)) {
				state = S9;
			} else if(i && ~d && (cnt <0x08)){
				state = increment;
				cnt++;
				t = 0;
			} else if(~i && d && (cnt <=0x01)) {
				state = S0;
			} else if(~i && d && (cnt > 0x01)){
				state = decrement;
				cnt--;
				t = 0;
			} else if(~i && ~d) {
				state = incrementWait;
			} 
			break;

		case decrement:
			if(d && i) {
				state = reset; 
			} else if (~i && d) {
				state = decrement;
				t++;
				if(t >= 10){
					t = 0;
					if(cnt > 0x00) {
						cnt--;
					} else {
						state = S0;
					}
				} 
			} else if (~d) {
				state = decrementWait;
			}
			break;

		case decrementWait: 
			if(i && d) {
				state = reset;
			} else if(i && ~d && (cnt>=0x08)) {
				state = S9;
			} else if(i && ~d && (cnt <0x08)) {
				state = increment;
				cnt++;
				t = 0;
			} else if(~i && d && (cnt <=0x01)){
				state = S0;
			} else if(~i && d && (cnt > 0x01))	{
				state = decrement;
				cnt--;
				t = 0;	
			} else if(~i && ~d) {
				state = decrementWait;
			}
			break;
		case S9:
			if(i && d) {
				state = reset;
			} else if(~i && d) {
				state = decrement;
				cnt--;
				t = 0;
			} else {
				state = S9;
			}
			break;
		
		default:
			break;
	}

	// state actions
	switch (state) {
		case Start:
			state = S0;
			break;
	
		case S0:
			cnt = 0x00;
			break; 

		case reset:
			cnt = 0x00;
			break;
		
		case increment:
                        break;

		case incrementWait:
                        break;

		case decrement:
			break;
		
		case decrementWait:
			break;

		case S9:
			cnt = 0x09;
			break;

		default:
			state = S0;
			break;
	}
}
 
int main(void) { 
    /* Insert DDR and PORT initializations */ 
    DDRA = 0x00; PORTA = 0xFF;		// configure pin A ports as input    
    DDRC = 0xFF; PORTC = 0x00;          // LCD data lines 
    DDRD = 0xFF; PORTD = 0x00;          // LCD control lines
	
    state = Start;              // start main with Start state. 
    TimerSet(100);              // set period to 100 ms
    TimerOn();                  // turn the timer on

    /* Initializes the LCD display */
    LCD_init();
    
    while (1) {
     	d = ~PINA & 0x02;
        i = ~PINA & 0x01; 

	counterTick(); 		// call the counterTick function

    	/* Starting at position 1 on the LCD screen, writes Hello World */
	
    	LCD_Cursor(1);
	LCD_WriteData( cnt + '0');
	
    	while (!TimerFlag) ;    // Wait 100 ms
    	TimerFlag = 0;          // lower the flag   
    }
 
    return 1; 
}

