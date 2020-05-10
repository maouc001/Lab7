/*      Author: Mahdi Aouchiche
 *      Partner(s) Name: 
 *      Lab Section: 22
 *      Assignment: Lab # 7;  Exercise #2 
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

enum State {Start, led1, led2, led3, press, pause} state;

unsigned char flag;	// represent restarting the game after pause 
unsigned char flag1;	// represents jump from states led1 and led3 to led2 
unsigned char LED;	// used for output
unsigned char cnt;	// used as a counter of LED2 hits
unsigned char b;	// button A0, b = ~PINA & 0x01

void tick() {
	// transitions
	switch (state) {
		case Start:
			LED = 0x01;
			cnt = 5;
			state = led1;
			break;
		case led1:
			if(!b || (b && flag == 1)) {
				state = led2;
				flag1 = 0;
			}
			else if (b && flag == 0) {
				state = press;
				if (cnt > 0) {
				       	cnt--;
				}
			}
			break;

		case led2:
                        if((!b && flag1 == 0)||(b && flag == 1 && flag1 == 0)) {
                                state = led3;
                        } else if((!b && flag1 == 1) || (b && flag == 1 && flag1 == 1 )){
				state = led1;
			} else if (b && flag == 0) {
                                state = press;
				if(cnt < 9) {
					cnt++;
				} 
                        }
                        break;

		case led3:
                        if(!b || (b && flag == 1)) {
                                state = led2;
				flag1 = 1;
                        }
                        else if (b && flag == 0) {
                                state = press;
				if (cnt > 0) {
                                        cnt--;
                                }
                        }
                        break;

		case press:
			if (b) {
				state = press;
			} else if(!b) {
				state = pause;

			} 
			break;

		case pause:
			if(!b) {
				state = pause;
			} else if (b) {
				state = led1;
				flag = 1;
				if(cnt >= 9){
					cnt = 5;
				} 
			}
			break;

		default:
			state = Start;
			break;
	}

	// state actions
	switch (state) {
		case led1:
			LED = 0x01;
			if(!b && flag == 1){ 
				flag = 0;
			}
			break;

		case led2:
			LED = 0x02;
			if(!b && flag == 1){
                                flag = 0;
                        }
			break; 

		case led3:
			LED = 0x04;
			if(!b && flag == 1){
                                flag = 0;
                        }
			break;
		case press:
			break;
		case pause:
			break;

		default:
			break;
	}
}

 
int main(void) { 
    /* Insert DDR and PORT initializations */ 
    DDRA = 0x00; PORTA = 0xFF;		// configure pin A ports as input
    DDRB = 0xFF; PORTB = 0x00;		// Set port B pins as outputs    
    DDRC = 0xFF; PORTC = 0x00;          // LCD data lines 
    DDRD = 0xFF; PORTD = 0x00;          // LCD control lines
	
    state = Start;              // start main with Start state. 
    TimerSet(300);              // set period to 300 ms
    TimerOn();                  // turn the timer on

    /* Initializes the LCD display */
    LCD_init();
    
    while (1) {
     
        b = ~PINA & 0x01; 	// button A0

	tick(); 		// call the tick function

    	/* Starting at position 1 on the LCD screen, writes cnt or Winner!!! */
	
    	if (cnt < 9) { 
		LCD_ClearScreen(); 
		LCD_Cursor(1);
		LCD_WriteData( cnt + '0');
	} else {
		LCD_DisplayString(1, "WINNER!!");
	} 	
 	
	PORTB = LED; 		// output LED to Port B 

    	while (!TimerFlag) ;    // Wait 300 ms
    	TimerFlag = 0;          // lower the flag   
    }
 
    return 1; 
}

