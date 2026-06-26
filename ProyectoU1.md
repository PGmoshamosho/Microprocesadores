# Reporte Proyecto 1 — Control de personaje en LCD con joystick analógico

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, el proyecto se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Este proyecto integra varios conceptos vistos a lo largo del curso en un solo programa: lectura analógica de dos canales (joystick), control de un display LCD 16x2, caracteres personalizados en CGRAM y lectura de un botón con anti-rebote, combinados para mover un personaje (sprite) por la pantalla del LCD y alternar su animación.

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
- Display LCD 16x2 (PORTC: RS=RC2, EN=RC3, D4-D7=RC4-RC7)
- Joystick analógico (2 potenciómetros) en AN0 (RA0, eje X) y AN1 (RA1, eje Y)
- Botón en RB0 (cambio de animación)
- Fuente 5 V (Arduino)

---

## 3. Descripción del Proyecto

| Componente | Puerto | Descripción |
|---|---|---|
| Joystick (2 pot.) | AN0 + AN1 (RA0, RA1) | Movimiento del personaje en X (columna) y Y (fila) |
| LCD 16x2 | PORTC | Despliegue del personaje (sprite) en su posición |
| Botón | RB0 | Alterna entre dos sprites: copa y estrella |

Se utilizó un joystick analógico conectado a los canales AN0 (eje X) y AN1 (eje Y) del PIC16F887. Moviendo el joystick hacia la derecha o izquierda, el eje X desplaza la columna del cursor en el LCD (0-15); moviendo el joystick hacia arriba o abajo, el eje Y selecciona entre la fila 0 o la fila 1 del display. Un botón en RB0 alterna entre dos caracteres personalizados (copa y estrella) creados en la CGRAM del LCD, funcionando como una especie de "personaje" que cambia de forma con cada pulsación.

<!-- ESPACIO PARA IMAGEN: Figura 1. Simulación del Proyecto -->

<!-- ESPACIO PARA IMAGEN: Figura 2. Actividad del Proyecto -->

<!-- ESPACIO PARA VIDEO: Video 1. Proyecto -->
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

void ADC_Init(){

    ANSEL = 0x03;
    ANSELH = 0x00;

    ADCON0 = 0x81;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(unsigned char channel){

    ADCON0 &= 0x83;
    ADCON0 |= channel << 2;
    __delay_ms(5);

    GO_nDONE = 1;

    while(GO_nDONE);
    return((ADRESH << 8) + ADRESL);
}

void Crear_Caracter(unsigned char posicion, unsigned char dibujo[]){
    unsigned char i;
    LCD_Cmd(0x40 + (posicion * 8));

    for(i = 0; i < 8; i++){
        LCD_putc(dibujo[i]);
    }

    LCD_Cmd(0x80);
}

void main(void){
    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    unsigned int adc_x;
    unsigned int adc_y;
    unsigned char columna = 7;
    unsigned char fila = 0;
    unsigned char animacion = 0;
    unsigned char estado_anterior = 1;

    ADC_Init();
    TRISBbits.TRISB0 = 1;
    OPTION_REGbits.nRBPU = 0;
    WPUBbits.WPUB0 = 1;

    LCD_Init(lcd);
    Crear_Caracter(0, copa);
    Crear_Caracter(1, estrella);

    while(1){
        adc_x = ADC_Read(0);
        adc_y = ADC_Read(1);

        if(adc_x > 600){
            columna++;
            if(columna > 15){
                columna = 0;
            }
        }
        else if(adc_x < 400){
            if(columna == 0){
                columna = 15;
            }
            else{
                columna--;
            }
        }

        if(adc_y < 400){
            fila = 0;
        }
        else if(adc_y > 600){
            fila = 1;
        }

        if(PORTBbits.RB0 == 0 && estado_anterior == 1){
            __delay_ms(50);

            if(PORTBbits.RB0 == 0){
                animacion++;

                if(animacion > 1){
                    animacion = 0;
                }
                while(PORTBbits.RB0 == 0);
                __delay_ms(50);
            }
        }
        estado_anterior = PORTBbits.RB0;

        LCD_Clear();
        LCD_Set_Cursor(fila, columna);
        if(animacion == 0){

            LCD_putc(0);
        }
        else{
            LCD_putc(1);
        }
        __delay_ms(150);
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
- Joystick: AN0=RA0 (eje X), AN1=RA1 (eje Y)

---

## 5. Conclusiones

Este proyecto integró varios módulos trabajados en prácticas anteriores —ADC de dos canales, manejo de LCD y caracteres personalizados, y lectura de botón con anti-rebote— en una sola aplicación interactiva. El uso de umbrales (400 y 600 sobre un rango 0-1023) para detectar la dirección del joystick permitió crear una "zona muerta" central que evita movimiento involuntario cuando el joystick está en reposo. Mover el cursor del LCD en tiempo real según la posición analógica mostró de forma práctica cómo un mismo módulo (LCD_Set_Cursor) puede usarse tanto para mostrar texto fijo como para crear una interacción dinámica tipo "juego".
