# Reporte 10 — Timer 1 (Actividad 1, Actividad 2)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el uso del Timer1 de 16 bits para medir el tiempo de ejecución del programa, mostrando el resultado en formato `mm:ss` en el LCD, y combinando esa medición de tiempo con la lectura analógica de un potenciómetro mostrada simultáneamente.

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
- Potenciómetro en AN0 (RA0) (Actividad 2)
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Act. 1 | LCD + Timer1 | PORTC (LCD) | Tiempo de ejecución en formato mm:ss |
| Act. 2 | LCD + Timer1 + Potenciómetro | PORTC (LCD) + AN0 (RA0) | Tiempo de ejecución + voltaje simultáneo |

### Actividad 1 — Tiempo de ejecución con Timer1
Se utilizó el Timer1 (registro de 16 bits, `TMR1H`/`TMR1L`) con interrupción para contar el tiempo transcurrido desde que el PIC empezó a ejecutarse, mostrando el resultado en formato `mm:ss` en el LCD.

<!-- ESPACIO PARA IMAGEN: Figura 1. Simulación de Actividad 1 -->

<!-- ESPACIO PARA IMAGEN: Figura 2. Actividad de Actividad 1 -->

<!-- ESPACIO PARA VIDEO: Video 1. Actividad 1 -->
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

volatile unsigned int tiempo = 0;
volatile unsigned int contador = 0;

char exec[6];

void Timer1_Init(){
    T1CON = 0x31;

    TMR1H = 0xF6;
    TMR1L = 0x3C;

    TMR1IF = 0;
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;
}

void __interrupt() ISR(void){
    if(TMR1IF){
        contador++;

        if(contador > 100){
            tiempo++;
            contador = 0;
        }

        TMR1H = 0xF6;
        TMR1L = 0x3C;

        TMR1IF = 0;
    }
}

void main(void){
    Timer1_Init();

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);
    LCD_Clear();

    LCD_Set_Cursor(0, 0);
    LCD_putrs("Tiempo: ");

    LCD_Set_Cursor(1, 0);
    LCD_putrs("00:00");

    while(1){
        LCD_Set_Cursor(1, 0);
        sprintf(exec, "%02u:%02u", tiempo / 60, tiempo % 60);
        LCD_putrs(exec);
    }
}
```
--------------------------------------------------------------------------------------------------------

### Actividad 2 — Voltaje de potenciómetro + tiempo con Timer1
Se combinó el Timer1 (medición de tiempo) con la lectura del ADC de un potenciómetro en AN0, mostrando el voltaje en la primera línea del LCD y el tiempo de ejecución al final de la segunda línea.

<!-- ESPACIO PARA IMAGEN: Figura 3. Simulación de Actividad 2 -->

<!-- ESPACIO PARA IMAGEN: Figura 4. Actividad de Actividad 2 -->

<!-- ESPACIO PARA VIDEO: Video 2. Actividad 2 -->
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

volatile unsigned int tiempo = 0;
volatile unsigned int contador = 0;

char exec[6];
char voltaje_txt[8];

void Timer1_Init(){
    T1CON = 0x31;

    TMR1H = 0xF6;
    TMR1L = 0x3C;

    TMR1IF = 0;
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;
}

void ADC_Init(){
    ANSEL = 0x01;
    ANSELH = 0x00;

    TRISAbits.TRISA0 = 1;

    ADCON0 = 0x81;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(){
    __delay_us(5);
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH << 8) + ADRESL);
}

void __interrupt() ISR(void){
    if(TMR1IF){
        contador++;

        if(contador > 100){
            tiempo++;
            contador = 0;
        }

        TMR1H = 0xF6;
        TMR1L = 0x3C;

        TMR1IF = 0;
    }
}

void main(void){
    unsigned int adc;
    unsigned int voltaje_mv;

    Timer1_Init();
    ADC_Init();

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);
    LCD_Clear();

    LCD_Set_Cursor(0, 0);
    LCD_putrs("Voltaje: ");

    while(1){
        adc = ADC_Read();

        voltaje_mv = (adc * 5000UL) / 1023;

        LCD_Set_Cursor(1, 0);
        sprintf(voltaje_txt, "%u.%02uV", voltaje_mv / 1000, (voltaje_mv % 1000) / 10);
        LCD_putrs(voltaje_txt);

        LCD_Set_Cursor(1, 11);
        sprintf(exec, "%02u:%02u", tiempo / 60, tiempo % 60);
        LCD_putrs(exec);

        __delay_ms(100);
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
- ADC (Actividad 2): AN0 = RA0 (potenciómetro)

---

## 5. Conclusiones

A diferencia del Timer0 (8 bits), el Timer1 trabaja con un registro de 16 bits (`TMR1H` y `TMR1L`), lo que permite contar intervalos más largos antes de desbordarse sin necesitar recargas tan frecuentes. El valor de precarga (0xF63C) se calculó para que cada interrupción equivalga a aproximadamente 10 ms con el prescaler configurado en `T1CON = 0x31`, acumulando hasta completar un segundo de la misma forma que en la práctica con Timer0. Esto permitió comparar directamente ambos periféricos para una misma tarea de medición de tiempo.
