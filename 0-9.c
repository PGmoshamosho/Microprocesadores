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

        PORTD = 0B00111111; // 0
        __delay_ms(500);

        PORTD = 0B00000110; // 1
        __delay_ms(500);

        PORTD = 0B01011011; // 2
        __delay_ms(500);

        PORTD = 0B01001111; // 3
        __delay_ms(500);

        PORTD = 0B01100110; // 4
        __delay_ms(500);

        PORTD = 0B01101101; // 5
        __delay_ms(500);

        PORTD = 0B01111101; // 6
        __delay_ms(500);

        PORTD = 0B00000111; // 7
        __delay_ms(500);

        PORTD = 0B01111111; // 8
        __delay_ms(500);

        PORTD = 0B01101111; // 9
        __delay_ms(500);
    }
}
