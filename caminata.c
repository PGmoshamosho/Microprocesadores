#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

void main(void) {

    TRISD = 0B00000000;
    PORTD = 0B00000000;

    while (1) {

        PORTD = 0B00000001;
        __delay_ms(300);

        PORTD = 0B00000010;
        __delay_ms(300);

        PORTD = 0B00000100;
        __delay_ms(300);

        PORTD = 0B00001000;
        __delay_ms(300);

        PORTD = 0B00010000;
        __delay_ms(300);

        PORTD = 0B00100000;
        __delay_ms(300);

        PORTD = 0B00010000;
        __delay_ms(300);

        PORTD = 0B00001000;
        __delay_ms(300);

        PORTD = 0B00000100;
        __delay_ms(300);

        PORTD = 0B00000010;
        __delay_ms(300);
    }
}