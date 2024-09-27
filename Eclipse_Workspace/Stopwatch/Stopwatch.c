/*
 * Stopwatch.c
 *
 *  Created on: Sep 14, 2024
 *      Author: Aya Yousef
 */

#include <avr/io.h> // Include the AVR I/O library
#include <util/delay.h> // Include the delay library
#include <avr/interrupt.h> // Include the interrupt library

// Define variables to store the current time
unsigned char second_count1 = 0; // Store the ones digit of the seconds
unsigned char second_count2 = 0; // Store the tens digit of the seconds
unsigned char minutes_count1 = 0; // Store the ones digit of the minutes
unsigned char minutes_count2 = 0; // Store the tens digit of the minutes
unsigned char hours_count1 = 0; // Store the ones digit of the hours
unsigned char hours_count2 = 0; // Store the tens digit of the hours

// Define flags to control the timer and counting
unsigned char flag_pause = 0; // Flag to pause the timer
unsigned char flag_count = 0; // Flag to control the counting
unsigned char hours_flag = 0; // Flag to control the hours counting

// Function to initialize the Timer1 in CTC mode
void Timer1_Init_CTC_Mode(void){
	/*
	 * FOC1A --> Non PWM
	 * WGM12 --> CTC
	 * CS10 & CS12 --> CLK/1024
	 */
	// Set the Timer1 to CTC mode
	TCCR1A = (1 << FOC1A);                                 // Set the FOC1A bit to 1
	    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12); // Set the WGM12, CS10, and CS12 bits to 1
	    TCNT1 = 0; // Initialize the counter register to 0
	    OCR1A = 15625; // Set the compare register to 15625 (1 second)
	    TIMSK |= (1 << OCIE1A); // Enable the interrupt module
}
// Interrupt Service Routine for Timer1
ISR(TIMER1_COMPA_vect) {
    // Check if the counting flag is 0
    if (flag_count == 0) {
        // Check if the pause flag is 0
        if (flag_pause == 0) {
            // Increment the seconds
            if (second_count1 == 9) {
                second_count1 = 0; // Reset the ones digit of the seconds
                second_count2++; // Increment the tens digit of the seconds

                // Check if the tens digit of the seconds is 6
                if (second_count2 == 6) {
                    second_count2 = 0; // Reset the tens digit of the seconds
                    minutes_count1++; // Increment the ones digit of the minutes

                    // Check if the ones digit of the minutes is 9
                    if (minutes_count1 == 9) {
                        minutes_count1 = 0; // Reset the ones digit of the minutes
                        minutes_count2++; // Increment the tens digit of the minutes

                        // Check if the tens digit of the minutes is 6
                        if (minutes_count2 == 6) {
                            minutes_count2 = 0; // Reset the tens digit of the minutes
                            hours_count1++; // Increment the ones digit of the hours

                            // Check if the ones digit of the hours is 9
                            if (hours_count1 == 9) {
                                hours_count1 = 0; // Reset the ones digit of the hours
                                hours_count2++; // Increment the tens digit of the hours
                            }
                        }
                    }
                }
            } else {
                second_count1++; // Increment the ones digit of the seconds
            }
        }
    } else {
        // Check if the pause flag is 0
        if (flag_pause == 0) {
            // Check if all the time variables are 0
            if ((second_count1 == 0) && (second_count2 == 0) && (minutes_count1 == 0) && (minutes_count2 == 0) && (hours_count1 == 0) && (hours_count2 == 0)) {
                PORTD |= (1 << PD0); // Set the PD0 pin high
            } else {
                // Decrement the seconds
                if (second_count1 == 0) {
                    second_count1 = 9; // Reset the ones digit of the seconds
                    if (second_count2 == 0) {
                        second_count2 = 5; // Reset the tens digit of the seconds
                        if (minutes_count1 == 0) {
                            minutes_count1 = 9; // Reset the ones digit of the minutes
                            if (minutes_count2 == 0) {
                                minutes_count2 = 5; // Reset the tens digit of the minutes
                                if (hours_count1 == 0) {
                                    hours_count1 = 9; // Reset the ones digit of the hours
                                    hours_count2--; // Decrement the tens digit of the hours
                                } else {
                                    hours_count1--; // Decrement the ones digit of the hours
                                }
                            } else {
                                minutes_count2--; // Decrement the tens digit of the minutes
                            }
                        } else {
                            minutes_count1--; // Decrement the ones digit of the minutes
                        }
                    } else {
                        second_count2--; // Decrement the tens digit of the seconds
                    }
                } else {
                    second_count1--; // Decrement the ones digit of the seconds
                }
            }
        }
    }
}

// Function to initialize the INT0 interrupt
void INT0_Init(void) {
    DDRD &= (~(1 << PD2)); // Configure the PD2 pin as an input
    PORTD |= (1 << PD2); // Enable the internal pull-up resistor
    MCUCR |= (1 << ISC01); // Trigger the INT0 interrupt on the falling edge
    GICR |= (1 << INT0); // Enable the INT0 interrupt
}

// Interrupt Service Routine for INT0
ISR(INT0_vect) {
    second_count1 = 0; // Reset the ones digit of the seconds
    second_count2 = 0; // Reset the tens digit of the seconds
    minutes_count1 = 0; // Reset the ones digit of the minutes
    minutes_count2 = 0; // Reset the tens digit of the minutes
    hours_count1 = 0; // Reset the ones digit of the hours
    hours_count2 = 0; // Reset the tens digit of the hours
}

// Function to initialize the INT1 interrupt
void INT1_Init(void) {
    DDRD &= (~(1 << PD3)); // Configure the PD3 pin as an input
    MCUCR |= (1 << ISC11) | (1 << ISC10); // Trigger the INT1 interrupt on the rising edge
    GICR |= (1 << INT1); // Enable the INT1 interrupt
}

// Interrupt Service Routine for INT1
ISR(INT1_vect) {
    flag_pause = 1; // Set the pause flag to 1
}

// Function to initialize the INT2 interrupt
void INT2_Init(void) {
    DDRB &= (~(1 << PB2)); // Configure the PB2 pin as an input
    PORTB |= (1 << PB2); // Enable the internal pull-up resistor
    MCUCSR &= ~(1 << ISC2); // Trigger the INT2 interrupt on the falling edge
    GICR |= (1 << INT2); // Enable the INT2 interrupt
}

// Interrupt Service Routine for INT2
ISR(INT2_vect) {
    flag_pause = 0; // Reset the pause flag to 0
}

// Function to toggle the mode
void Toggle_Mode(void) {
    DDRB &= ~(1 << PB7); // Configure the PB7 pin as an input
    PORTB |= (1 << PB7); // Enable the internal pull-up resistor
    if (!(PINB & (1 << PB7))) { // Check if the PB7 pin is low
       if(flag_count == 0){ 		//check if flag = 0;
            PORTD |= (1 << PD5); // Set the counting flag to 1
            PORTD &= ~(1 << PD4);	// Set the PD5 pin high
            flag_count = 1 ;	//set flag to 1
        }
    } else {
        flag_count = 0; // Reset the counting flag to 0
        PORTD |= (1 << PD4); // Set the PD4 pin high
        PORTD &= ~(1 << PD5); // Reset the PD5 pin
        PORTD &= ~(1 << PD0); // Reset the PD0 pin
    }
}
// Function to adjust the hours
void adjust_hours(void) {
    static unsigned char hour_increment_flag = 1; // Flag to increment the hours
    static unsigned char hour_decrement_flag = 1; // Flag to decrement the hours

    DDRB &= (~(1 << PB0)) & (~(1 << PB1)); // Configure the PB0 and PB1 pins as inputs
    PORTB |= (1 << PB0) | (1 << PB1); // Enable the internal pull-up resistors

    if (!(PINB & (1 << PB1))) { // Check if the increment hours button is pressed
        _delay_ms(30); // Debounce delay
        if (!(PINB & (1 << PB1)) && hour_increment_flag) { // Confirm button press
            hour_increment_flag = 0; // Reset the increment flag
            if (hours_count1 == 9) {
                hours_count1 = 0; // Reset the ones digit of the hours
                if (hours_count2 == 9) {
                    hours_count2 = 0; // Reset the tens digit of the hours
                } else {
                    hours_count2++; // Increment the tens digit of the hours
                }
            } else {
                hours_count1++; // Increment the ones digit of the hours
            }
        }
    } else {
        hour_increment_flag = 1; // Set the increment flag when the button is not pressed
    }

    if (!(PINB & (1 << PB0))) { // Check if the decrement hours button is pressed
        _delay_ms(30); // Debounce delay
        if (!(PINB & (1 << PB0)) && hour_decrement_flag) { // Confirm button press
            hour_decrement_flag = 0; // Reset the decrement flag
            if (hours_count1 == 0) {
                hours_count1 = 9; // Reset the ones digit of the hours
                if (hours_count2 == 0) {
                    hours_count2 = 9; // Reset the tens digit of the hours
                } else {
                    hours_count2--; // Decrement the tens digit of the hours
                }
            } else {
                hours_count1--; // Decrement the ones digit of the hours
            }
        }
    } else {
        hour_decrement_flag = 1; // Set the decrement flag when the button is not pressed
    }
    _delay_ms(10); // Add a delay to avoid busy-waiting
}
// Function to adjust the minutes.
void adjust_minutes(void) {
    // Define flags to prevent multiple increments or decrements
    static unsigned char minute_increment_flag = 1;
    static unsigned char minute_decrement_flag = 1;

    DDRB &= (~(1<<PB3)) & (~(1<<PB4));// Configure PB3 and PB4 as input pins
    PORTB |= 1<<PB3 | 1<<PB4;// Enable internal pull-up resistors for PB3 and PB4

    if (!(PINB & (1 << PB4))) {    // Check if increment minutes button is pressed
        _delay_ms(30);              // Debounce delay to prevent multiple increments
        if (!(PINB & (1 << PB4)) && minute_increment_flag) {   // Confirm button press
            minute_increment_flag = 0;   // Reset increment flag
            if (minutes_count1 == 9) { // Check if minutes_count1 is 9
                minutes_count1 = 0; // Reset minutes_count1
                if (minutes_count2 == 9) { // Check if minutes_count2 is 9
                    minutes_count2 = 0; // Reset minutes_count2
                } else {
                    minutes_count2++;   // Increment minutes_count2
                }
            } else {
                minutes_count1++; // Increment minutes_count1
            }
        }
    } else {
        minute_increment_flag = 1;// Set increment flag when button is not pressed
    }
    if (!(PINB & (1 << PB3))) { // Check if decrement minutes button is pressed
        _delay_ms(30);// Debounce delay to prevent multiple decrements
        if (!(PINB & (1 << PB3)) && minute_decrement_flag) { // Confirm button press
            minute_decrement_flag = 0;// Reset decrement flag
            if (minutes_count1 == 0) {// Check if minutes_count1 is 0
                minutes_count1 = 9;// Reset minutes_count1
                if (minutes_count2 == 0) {// Check if minutes_count2 is 0
                    minutes_count2 = 9;// Reset minutes_count2
                } else {
                    minutes_count2--; // Decrement minutes_count2
                }
            } else {
                minutes_count1--;// Decrement minutes_count1
            }
        }
    } else {
        minute_decrement_flag = 1;// Set decrement flag when button is not pressed
    }
    _delay_ms(10); // Add a delay to avoid busy-waiting
}

void adjust_seconds(void) {
    // Define flags to prevent multiple increments or decrements
    static unsigned char second_increment_flag = 1;
    static unsigned char second_decrement_flag = 1;

    DDRB &= (~(1<<PB5)) & (~(1<<PB6));// Configure PB5 and PB6 as input pins
    PORTB |= 1<<PB5 | 1<<PB6;// Enable internal pull-up resistors for PB5 and PB6
    if (!(PINB & (1 << PB6))) {// Check if increment seconds button is pressed
        _delay_ms(30);        // Debounce delay to prevent multiple increments
        if (!(PINB & (1 << PB6)) && second_increment_flag) {        // Confirm button press
            second_increment_flag = 0;            // Reset increment flag
            if (second_count1 == 9) {            // Check if second_count1 is 9
                second_count1 = 0;                // Reset second_count1
                if (second_count2 == 9) {                // Check if second_count2 is 9
                    second_count2 = 0;                    // Reset second_count2
                } else {
                    second_count2++;                    // Increment second_count2
                }
            } else {
                second_count1++;                // Increment second_count1
            }
        }
    } else {
        second_increment_flag = 1; // Set increment flag when button is not pressed
    }


    if (!(PINB & (1 << PB5))) { // Check if decrement seconds button is pressed
        _delay_ms(30);  // Debounce delay to prevent multiple decrements
        if (!(PINB & (1 << PB5)) && second_decrement_flag) {// Confirm button press
            second_decrement_flag = 0;// Reset decrement flag
            if (second_count1 == 0) {// Check if second_count1 is 0
                second_count1 = 9;  // Reset second_count1
                if (second_count2 == 0) {// Check if second_count2 is 0
                    second_count2 = 9;// Reset second_count2
                } else {
                    second_count2--;  // Decrement second_count2
                }
            } else {
                second_count1--; // Decrement second_count1
            }
        }
    } else {
        second_decrement_flag = 1;// Set decrement flag when button is not pressed
    }
    _delay_ms(10);  // Add a delay to avoid busy-waiting
}

int main (void){

    Timer1_Init_CTC_Mode();// Initialize Timer1 in CTC mode
    INT0_Init();// Initialize INT0 interrupt
    INT1_Init(); // Initialize INT1 interrupt
    INT2_Init();   // Initialize INT2 interrupt
    SREG |= 1<<7;// Enable global interrupts
    DDRC  |= 0x0F;// Configure the first four pins in PORTC as output pins
    DDRA  |= 0x3F;// Configure the first six pins in PORTA as output pins
    // Initialize PORTC and PORTA
    PORTC &= 0xF0;
    PORTA &= 0xC0;
    DDRD |= (1<<PD4) |(1<<PD5) | (1<<PD0);    // Configure PD4, PD5, and PD0 as output pins
    while(1) {
        // Display the seconds on PORTC
        // Clear the lower 6 bits of PORTA and set PA5 high
        PORTA = (PORTA & 0xC0) | 1<<PA5;
        PORTC = (PORTC & 0xF0) | second_count1;        // Display the ones digit of the seconds on PORTC
        _delay_ms(1);        // Delay for 1 millisecond
        // Display the tens digit of the seconds on PORTC
        // Clear the lower 6 bits of PORTA and set PA4 high
        PORTA = (PORTA & 0xC0) | 1<<PA4;
        PORTC = (PORTC & 0xF0) | second_count2;
        _delay_ms(3);        // Delay for 1 millisecond
        // Display the minutes on PORTC
        // Clear the lower 6 bits of PORTA and set PA3 high
        PORTA = (PORTA & 0xC0) | 1<<PA3;
        PORTC = (PORTC & 0xF0) | minutes_count1;// Display the ones digit of the minutes on PORTC
        _delay_ms(3);        // Delay for 1 millisecond
        // Display the tens digit of the minutes on PORTC
        // Clear the lower 6 bits of PORTA and set PA2 high
        PORTA = (PORTA & 0xC0) | 1<<PA2;
        PORTC = (PORTC & 0xF0) | minutes_count2;        // Display the tens digit of the minutes on PORTC
        _delay_ms(3);         // Delay for 1 millisecond
        // Display the hours on PORTC
        // Clear the lower 6 bits of PORTA and set PA1 high
        PORTA = (PORTA & 0xC0) | 1<<PA1;
        PORTC = (PORTC & 0xF0) | hours_count1;        // Display the ones digit of the hours on PORTC
        _delay_ms(3);        // Delay for 1 millisecond
        // Display the tens digit of the hours on PORTC
        // Clear the lower 6 bits of PORTA and set PA0 high
        PORTA = (PORTA & 0xC0) | 1<<PA0;
        PORTC = (PORTC & 0xF0) | hours_count2;        // Display the tens digit of the hours on PORTC
        _delay_ms(1);        // Delay for 1 millisecond

        Toggle_Mode (); // Toggle the mode
        adjust_hours(); // Adjust the hours
        adjust_minutes();// Adjust the minutes
        adjust_seconds();// Adjust the seconds
    }
}
