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
    TRISD = 0x00;
    PORTD = 0x00;
    
    unsigned char contador = 0;
    
    while(1) {
        PORTD = contador;
        __delay_ms(1000);
        contador++;
        if(contador > 15) contador = 0;
    }
}