#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 8000000

void main(void) {

    TRISB = 0b00000000;
    TRISD = 0b00000000;

    PORTB = 0b11111111;
    PORTD = 0b00000000;

    while(1) {

        PORTB = 0b11111110;
        PORTD = 0b10000001;
        __delay_ms(2);

        PORTB = 0b11111101;
        PORTD = 0b01000010;
        __delay_ms(2);

        PORTB = 0b11111011;
        PORTD = 0b00100100;
        __delay_ms(2);

        PORTB = 0b11110111;
        PORTD = 0b00011000;
        __delay_ms(2);

        PORTB = 0b11101111;
        PORTD = 0b00011000;
        __delay_ms(2);

        PORTB = 0b11011111;
        PORTD = 0b00100100;
        __delay_ms(2);

        PORTB = 0b10111111;
        PORTD = 0b01000010;
        __delay_ms(2);

        PORTB = 0b01111111;
        PORTD = 0b10000001;
        __delay_ms(2);
    }
}