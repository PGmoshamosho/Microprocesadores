# Reporte 8 — Múltiple Lectura Analógica (Actividad 1, Actividad 2)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren la lectura simultánea de dos potenciómetros usando dos canales del ADC (AN0 y AN1), mostrando ambos valores de voltaje en el LCD, y una variante con vistas intercambiables (voltaje, porcentaje, ADC) seleccionables de forma independiente para cada potenciómetro.

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
- Dos potenciómetros en AN0 (RA0) y AN1 (RA1)
- Botones en RB0 y RB1 (Actividad 2)
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Act. 1 | 2 potenciómetros + LCD | AN0+AN1 (RA0,RA1) + PORTC (LCD) | Voltaje de 2 potenciómetros simultáneo |
| Act. 2 | 2 potenciómetros + LCD + 2 botones | AN0+AN1 + PORTC (LCD) + RB0,RB1 (botones) | Vista intercambiable por cada potenciómetro |

### Actividad 1 — Voltaje de dos potenciómetros en el LCD
Se conectaron dos potenciómetros a los canales AN0 y AN1, mostrando ambos valores de voltaje simultáneamente, uno en cada línea del display LCD.

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

void ADC_Init(){
    ANSEL = 0x03;
    ANSELH = 0x00;

    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;

    ADCON0 = 0x01;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(unsigned char channel){
    __delay_ms(5);

    ADCON0 &= 0x83;
    ADCON0 |= channel << 2;

    __delay_ms(2);

    GO_nDONE = 1;
    while(GO_nDONE);

    return((ADRESH << 8) + ADRESL);
}

void main(void){
    char buffer1[16];
    char buffer2[16];

    unsigned int adc_result1;
    unsigned int adc_result2;

    unsigned int volt1;
    unsigned int volt2;

    unsigned int part_ent1;
    unsigned int part_dec1;

    unsigned int part_ent2;
    unsigned int part_dec2;

    ADC_Init();

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);

    while(1){
        adc_result1 = ADC_Read(0);
        adc_result2 = ADC_Read(1);

        volt1 = (adc_result1 * 5000) / 1023;
        volt2 = (adc_result2 * 5000) / 1023;

        part_ent1 = volt1 / 1000;
        part_dec1 = volt1 % 1000;

        part_ent2 = volt2 / 1000;
        part_dec2 = volt2 % 1000;

        LCD_Clear();

        LCD_Set_Cursor(0,0);
        sprintf(buffer1, "Voltaje 1:%u.%03u", part_ent1, part_dec1);
        LCD_putrs(buffer1);

        LCD_Set_Cursor(1,0);
        sprintf(buffer2, "Voltaje 2:%u.%03u", part_ent2, part_dec2);
        LCD_putrs(buffer2);

        __delay_ms(300);
    }
}
```
--------------------------------------------------------------------------------------------------------

### Actividad 2 — Vista intercambiable por cada potenciómetro
Se agregaron dos botones independientes (RB0 y RB1), uno para cambiar la vista (voltaje, porcentaje o ADC) del primer potenciómetro y otro para cambiar la vista del segundo, mostrando ambos resultados simultáneamente en el LCD.

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

void main(void){
    ADC_Init();

    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;

    OPTION_REGbits.nRBPU = 0;
    WPUBbits.WPUB0 = 1;
    WPUBbits.WPUB1 = 1;

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);

    char buffer1[16];
    char buffer2[16];

    unsigned char modo1 = 0;
    unsigned char modo2 = 0;

    unsigned char estado_anterior1 = 1;
    unsigned char estado_anterior2 = 1;

    while(1){

        if(PORTBbits.RB0 == 0 && estado_anterior1 == 1){
            __delay_ms(50);

            if(PORTBbits.RB0 == 0){
                modo1++;

                if(modo1 > 2){
                    modo1 = 0;
                }

                while(PORTBbits.RB0 == 0);
                __delay_ms(50);
            }
        }

        if(PORTBbits.RB1 == 0 && estado_anterior2 == 1){
            __delay_ms(50);

            if(PORTBbits.RB1 == 0){
                modo2++;

                if(modo2 > 2){
                    modo2 = 0;
                }

                while(PORTBbits.RB1 == 0);
                __delay_ms(50);
            }
        }

        estado_anterior1 = PORTBbits.RB0;
        estado_anterior2 = PORTBbits.RB1;

        unsigned int adc_result1 = ADC_Read(0);
        unsigned int adc_result2 = ADC_Read(1);

        LCD_Clear();

        LCD_Set_Cursor(0,0);

        if(modo1 == 0){
            unsigned long volt1 = ((unsigned long)adc_result1 * 50000) / 1023;
            unsigned int part_int1 = volt1 / 10000;
            unsigned int part_dec1 = volt1 % 10000;

            sprintf(buffer1, "V1:%u.%04uV", part_int1, part_dec1);
            LCD_putrs(buffer1);
        }
        else if(modo1 == 1){
            unsigned int porcentaje1 = ((unsigned long)adc_result1 * 100) / 1023;

            sprintf(buffer1, "P1:%u %%", porcentaje1);
            LCD_putrs(buffer1);
        }
        else{
            sprintf(buffer1, "ADC1:%u", adc_result1);
            LCD_putrs(buffer1);
        }

        LCD_Set_Cursor(1,0);

        if(modo2 == 0){
            unsigned long volt2 = ((unsigned long)adc_result2 * 50000) / 1023;
            unsigned int part_int2 = volt2 / 10000;
            unsigned int part_dec2 = volt2 % 10000;

            sprintf(buffer2, "V2:%u.%04uV", part_int2, part_dec2);
            LCD_putrs(buffer2);
        }
        else if(modo2 == 1){
            unsigned int porcentaje2 = ((unsigned long)adc_result2 * 100) / 1023;

            sprintf(buffer2, "P2:%u %%", porcentaje2);
            LCD_putrs(buffer2);
        }
        else{
            sprintf(buffer2, "ADC2:%u", adc_result2);
            LCD_putrs(buffer2);
        }

        __delay_ms(200);
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
- ADC: AN0 = RA0 (potenciómetro 1), AN1 = RA1 (potenciómetro 2)

---

## 5. Conclusiones

Trabajar con dos canales del ADC simultáneamente mostró cómo seleccionar el canal activo mediante los bits CHS de ADCON0 antes de cada conversión. Manejar dos botones con sus respectivas banderas de estado anterior permitió cambiar la vista de cada potenciómetro de forma totalmente independiente sin que las lecturas interfirieran entre sí.
