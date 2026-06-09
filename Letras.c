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

void mostrarO(void){
    int i;
    for(i=0; i<80; i++){
        PORTD = 0B00000001; PORTB = 0B11000011; __delay_ms(2);
        PORTD = 0B00000010; PORTB = 0B10111101; __delay_ms(2);
        PORTD = 0B00000100; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B00001000; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B00010000; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B00100000; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B01000000; PORTB = 0B10111101; __delay_ms(2);
        PORTD = 0B10000000; PORTB = 0B11000011; __delay_ms(2);
    }
}

void mostrarR(void){
    int i;
    for(i=0; i<80; i++){
        PORTD = 0B00000001; PORTB = 0B00000011; __delay_ms(2);
        PORTD = 0B00000010; PORTB = 0B01111101; __delay_ms(2);
        PORTD = 0B00000100; PORTB = 0B01111101; __delay_ms(2);
        PORTD = 0B00001000; PORTB = 0B00000011; __delay_ms(2);
        PORTD = 0B00010000; PORTB = 0B01011111; __delay_ms(2);
        PORTD = 0B00100000; PORTB = 0B01101111; __delay_ms(2);
        PORTD = 0B01000000; PORTB = 0B01110111; __delay_ms(2);
        PORTD = 0B10000000; PORTB = 0B01111011; __delay_ms(2);
    }
}

void mostrarH(void){
    int i;
    for(i=0; i<80; i++){
        PORTD = 0B00000001; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B00000010; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B00000100; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B00001000; PORTB = 0B00000000; __delay_ms(2);
        PORTD = 0B00010000; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B00100000; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B01000000; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B10000000; PORTB = 0B01111110; __delay_ms(2);
    }
}

void mostrarM(void){
    int i;
    for(i=0; i<80; i++){
        PORTD = 0B00000001; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B00000010; PORTB = 0B00111100; __delay_ms(2);
        PORTD = 0B00000100; PORTB = 0B01011010; __delay_ms(2);
        PORTD = 0B00001000; PORTB = 0B01100110; __delay_ms(2);
        PORTD = 0B00010000; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B00100000; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B01000000; PORTB = 0B01111110; __delay_ms(2);
        PORTD = 0B10000000; PORTB = 0B01111110; __delay_ms(2);
    }
}

void main(void) {

    TRISB = 0B00000000;
    TRISD = 0B00000000;

    PORTB = 0B11111111;
    PORTD = 0B00000000;

    while(1){

        mostrarO();
        __delay_ms(300);

        mostrarR();
        __delay_ms(300);

        mostrarH();
        __delay_ms(300);

        mostrarM();
        __delay_ms(300);
    }
}