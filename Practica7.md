# Reporte 7 — Lectura Analógica (Actividad 1)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

La práctica de este reporte cubre la lectura del ADC de 10 bits del PIC16F887 sobre un potenciómetro conectado a AN0, mostrando el resultado en el LCD con tres vistas intercambiables: voltaje, porcentaje y valor crudo del ADC.

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
- Potenciómetro en AN0 (RA0)
- Botón en RB0 (cambio de vista)
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Act. 1 | Potenciómetro + LCD + botón | AN0 (RA0) + PORTC (LCD) + RB0 (botón) | Vista intercambiable Voltaje / Porcentaje / ADC |

### Actividad 1 — Vista intercambiable: Voltaje / Porcentaje / ADC
Se conectó un potenciómetro al canal AN0 del PIC16F887. Con un botón en RB0 se alterna entre tres formas de mostrar la lectura en el LCD: voltaje (V), porcentaje (%) y valor crudo del ADC (0-1023).

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
    ANSEL = 0x01;
    ANSELH = 0x00;

    ADCON0 = 0x81;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(){
    __delay_us(5);
    GO_nDONE = 1;
    while(GO_nDONE);
    return((ADRESH << 8) + ADRESL);
}

void main(void){
    ADC_Init();

    TRISBbits.TRISB0 = 1;
    OPTION_REGbits.nRBPU = 0;
    WPUBbits.WPUB0 = 1;

    LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};
    LCD_Init(lcd);

    char buffer[16];
    unsigned char modo = 0;
    unsigned char estado_anterior = 1;

    while(1){

        if(PORTBbits.RB0 == 0 && estado_anterior == 1){
            __delay_ms(50);

            if(PORTBbits.RB0 == 0){
                modo++;

                if(modo > 2){
                    modo = 0;
                }

                while(PORTBbits.RB0 == 0);
                __delay_ms(50);
            }
        }

        estado_anterior = PORTBbits.RB0;

        unsigned int adc_result = ADC_Read();

        LCD_Clear();
        LCD_Set_Cursor(0,0);

        if(modo == 0){
            unsigned long volt = ((unsigned long)adc_result * 50000) / 1023;
            unsigned int part_int = volt / 10000;
            unsigned int part_dec = volt % 10000;

            LCD_putrs("Voltaje:");
            LCD_Set_Cursor(1,0);
            sprintf(buffer, "%u.%04u V", part_int, part_dec);
            LCD_putrs(buffer);
        }
        else if(modo == 1){
            unsigned int porcentaje = ((unsigned long)adc_result * 100) / 1023;

            LCD_putrs("Porcentaje:");
            LCD_Set_Cursor(1,0);
            sprintf(buffer, "%u %%", porcentaje);
            LCD_putrs(buffer);
        }
        else{
            LCD_putrs("ADC:");
            LCD_Set_Cursor(1,0);
            sprintf(buffer, "%u", adc_result);
            LCD_putrs(buffer);
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
- ADC: AN0 = RA0 (potenciómetro)

---

## 5. Conclusiones

La lectura analógica con el módulo ADC del PIC16F887 permitió convertir una señal de voltaje continua del potenciómetro en un valor digital de 10 bits, que después se transformó matemáticamente a voltaje real y porcentaje. El uso de un botón con anti-rebote para alternar entre las tres vistas mostró cómo combinar entradas digitales con la lectura analógica en un mismo programa sin que se interfieran entre sí.
