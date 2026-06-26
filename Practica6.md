# Reporte 6 — Display LCD 16x2 (Clase, Actividad 1)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el manejo del display LCD 16x2 en modo de 4 bits a través de PORTC, mostrando texto fijo y caracteres personalizados (CGRAM) seleccionables con un botón.

---

## 2. Desarrollo

### Software
- MPLAB IDE X
- Compilador XC8
- Proteus Design Suite

### Hardware
- PIC16F887
- PICkit (ICSP)
- Cristal 8 MHz + botón de reinicio
- Resistencia pull-up 1 kΩ en MCLR
- Display LCD 16x2 (modo 4 bits, PORTC: RS=RC2, EN=RC3, D4-D7=RC4-RC7)
- Botón en RB0 (Actividad 1)
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Clase | LCD 16x2 | PORTC | "Hello World!" + recorrido de letras A-P |
| Act. 1 | LCD 16x2 + botón | PORTC (LCD) + PORTB (botón) | Caracteres personalizados (copa, estrella, corazón) |

### Clase — "Hello World!" en el LCD
Se conectó un display LCD 16x2 en modo de 4 bits al PORTC del PIC16F887 (RS=RC2, EN=RC3, D4-D7=RC4-RC7) y se mostró el texto "HELLO WORLD!" en la primera línea, recorriendo las letras de la A a la P en la segunda línea.

<img width="809" height="451" alt="image" src="https://github.com/user-attachments/assets/91c91de1-c868-40c7-94a5-e16df46ef90f" />

Figura 1

--------------------------------------------------------------------------------------------------------
## Codigo
```c
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lcd.h"

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

void main(void){

    char c;
    LCD display = {&PORTC, 2, 3, 4, 5, 6, 7};

    ANSEL = 0x00;
    ANSELH = 0x00;
    PORTC = 0x00;

    LCD_Init(display);

    while(1){

        LCD_Clear();
        LCD_Set_Cursor(0,0);
        LCD_putrs(" HELLO WORLD! ");

        LCD_Set_Cursor(1,0);

        for(c = 'A'; c < 'Q'; c++){
            LCD_putc(c);
            __delay_ms(300);
        }

        __delay_ms(1000);
    }
}
```

**lcd.h** (interfaz de la librería LCD usada en todas las prácticas con display):
```c
#ifndef LCD_H
#define LCD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    volatile unsigned char* PORT; // Pointer to the LCD port e.g &PORTC
    unsigned RS :3;
    unsigned EN :3;
    unsigned D4 :3;
    unsigned D5 :3;
    unsigned D6 :3;
    unsigned D7 :3;
} LCD;

extern LCD lcd;

#define LCD_Clear() LCD_Cmd(0x01)
#define LCD_Decrement_Cursor() LCD_Cmd(0x04)
#define LCD_Increment_Cursor() LCD_Cmd(0x05)
#define LCD_Shift_Display_Right() LCD_Cmd(0x06)
#define LCD_Shift_Display_Left() LCD_Cmd(0x07)
#define LCD_Shift_Right() LCD_Cmd(0x1C)
#define LCD_Shift_Left() LCD_Cmd(0x18)

#define LCD_Set_Cursor( x, y ) \
do { \
    if ( x == 0 ) { \
        LCD_Cmd(0x80 + y); \
    } \
    else if ( x == 1 ) { \
        LCD_Cmd(0xC0 + y); \
    } \
    else if ( x == 2 ) { \
        LCD_Cmd(0x94 + y); \
    } \
    else if ( x == 3 ) { \
        LCD_Cmd(0xD4 + y); \
    } \
} while ( false )

#define LCD_Display( on, cursor, blink ) \
do { \
    unsigned char cmd = 0x08; \
    if ( on == true ) { cmd |= 1 << 2; } \
    if ( cursor == true ) { cmd |= 1 << 1; } \
    if ( blink == true ) { cmd |= 1; } \
    LCD_Cmd(cmd); \
} while ( false )

#define LCD_Cmd( c ) \
do { \
    LCD_Write( (c & 0xF0) >> 4 ); \
    LCD_Write( c & 0x0F); \
} while ( false )

bool LCD_Init ( LCD display );
void LCD_putc ( char a );
void LCD_puts ( char *a );
void LCD_putrs ( const char *a );
void LCD_Write ( unsigned char c );
void LCD_Out ( char a );

#ifdef __cplusplus
}
#endif

#endif /* LCD_H */
```
--------------------------------------------------------------------------------------------------------

### Actividad 1 — Caracteres personalizados (copa, estrella, corazón)
Se crearon tres caracteres personalizados en la CGRAM del LCD (copa, estrella y corazón) y se utilizó un botón en RB0 para alternar entre los tres mensajes, cada uno mostrando el carácter correspondiente.

<img width="776" height="416" alt="image" src="https://github.com/user-attachments/assets/222baaac-74d5-43eb-8553-446fc32da025" />

Figura 2

<img width="748" height="386" alt="image" src="https://github.com/user-attachments/assets/13a34416-1b4c-4589-8762-538624c237f2" />

Figura 3

<img width="637" height="303" alt="image" src="https://github.com/user-attachments/assets/07a8956d-de11-4ce9-b68a-583a93303930" />

Figura 4

--------------------------------------------------------------------------------------------------------
## Codigo
```c
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lcd.h"

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

unsigned char copa[8] = {
    0x0E,
    0x1F,
    0x15,
    0x1F,
    0x0E,
    0x04,
    0x0E,
    0x1F
};

unsigned char estrella[8] = {
    0x04,
    0x15,
    0x0E,
    0x1F,
    0x0E,
    0x15,
    0x04,
    0x00
};

unsigned char corazon[8] = {
    0x00,
    0x0A,
    0x1F,
    0x1F,
    0x1F,
    0x0E,
    0x04,
    0x00
};

void Crear_Caracter(unsigned char posicion, unsigned char caracter[]){
    unsigned char i;

    LCD_Cmd(0x40 + (posicion * 8));

    for(i = 0; i < 8; i++){
        LCD_putc(caracter[i]);
    }

    LCD_Cmd(0x80);
}

void Mostrar_Mensaje(unsigned char figura){

    LCD_Clear();

    LCD_Set_Cursor(0,0);
    LCD_putrs("Campeon");

    LCD_Set_Cursor(1,0);
    LCD_putrs("MEXICO!! ");
    LCD_putc(figura);
}

void main(void){

    unsigned char estado;
    unsigned char boton_anterior;

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};

    ANSEL = 0x00;
    ANSELH = 0x00;

    estado = 0;
    boton_anterior = 1;

    TRISBbits.TRISB0 = 1;

    OPTION_REGbits.nRBPU = 0;
    WPUBbits.WPUB0 = 1;

    LCD_Init(lcd);

    Crear_Caracter(0, copa);
    Crear_Caracter(1, estrella);
    Crear_Caracter(2, corazon);

    Mostrar_Mensaje(0);

    while(1){

        if(PORTBbits.RB0 == 0 && boton_anterior == 1){
            __delay_ms(50);

            if(PORTBbits.RB0 == 0){

                estado++;

                if(estado > 2){
                    estado = 0;
                }

                Mostrar_Mensaje(estado);
            }
        }

        boton_anterior = PORTBbits.RB0;
    }
}
```
--------------------------------------------------------------------------------------------------------

## 4. Circuito Base

- VDD (pines 11, 32) → +5 V | VSS (pines 12, 31) → GND
- R1 = 1 kΩ entre MCLR (pin 1) y VDD
- Cristal 8 MHz en RA6 (pin 14) y RA7 (pin 13)
- Configuration bits: oscilador HS, MCLRE habilitado, WDT deshabilitado
- LCD: RS=RC2, EN=RC3, D4=RC4, D5=RC5, D6=RC6, D7=RC7

---

## 5. Conclusiones

El manejo del LCD en modo 4 bits permitió comprender el protocolo de comunicación basado en pulsos de habilitación (EN) y el envío de nibbles altos y bajos por separado. La creación de caracteres personalizados en la CGRAM amplía considerablemente lo que se puede mostrar en pantalla más allá del set de caracteres ASCII estándar del controlador HD44780.
