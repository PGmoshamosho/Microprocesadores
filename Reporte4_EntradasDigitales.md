# Reporte 4 — Entradas Digitales (Clase, Actividad 1)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el manejo de entradas digitales: 3 botones controlando 3 LEDs usando pull-ups internas, y un contador de 0-99 con dos displays de 7 segmentos controlado por botones de suma, resta y reset.

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
- Resistencias 330 Ω
- 3 botones (Clase) / 3 botones + 2 displays 7 seg. (Actividad 1)
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Clase | 3 botones + 3 LEDs | PORTB (botones) + PORTD (LEDs) | Control de 3 LEDs con pull-up interno |
| Act. 1 | 2 displays 7 seg. + 3 botones | PORTB (botones) + PORTC (selección) + PORTD (segmentos) | Contador 0-99 con suma, resta y reset |

### Clase — 3 Botones controlando 3 LEDs (Pull-up interno)
Se conectaron 3 botones al PORTB (RB0, RB1, RB2) configurados como entradas con resistencias de pull-up internas activadas, y 3 LEDs al PORTD (RD0, RD1, RD2). Al presionar cada botón se enciende el LED correspondiente.

<!-- ESPACIO PARA IMAGEN: Figura 1. Simulación de Clase -->

<!-- ESPACIO PARA IMAGEN: Figura 2. Actividad de Clase -->

<!-- ESPACIO PARA VIDEO: Video 1. Clase -->
--------------------------------------------------------------------------------------------------------
## Codigo
```c
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
    TRISB = 0B00000111;
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0B00000111;

    TRISD = 0x00;
    PORTD = 0x00;

    ANSEL = 0x00;
    ANSELH = 0x00;

    while (1) {
        if (RB0 == 0) RD0 = 1;
        else RD0 = 0;

        if (RB1 == 0) RD1 = 1;
        else RD1 = 0;

        if (RB2 == 0) RD2 = 1;
        else RD2 = 0;
    }
}
```
--------------------------------------------------------------------------------------------------------

### Actividad 1 — Contador 0-99 con dos displays de 7 segmentos
Se implementó un contador de 0 a 99 usando dos displays de 7 segmentos multiplexados. Un botón suma una unidad, otro resta una unidad y un tercero reinicia el contador a cero.

<!-- ESPACIO PARA IMAGEN: Figura 3. Simulación de Actividad 1 -->

<!-- ESPACIO PARA IMAGEN: Figura 4. Actividad de Actividad 1 -->

<!-- ESPACIO PARA VIDEO: Video 2. Actividad 1 -->
--------------------------------------------------------------------------------------------------------
## Codigo
```c
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
#define BTN_SUMA   RB0
#define BTN_RESTA  RB1
#define BTN_RESET  RB2

unsigned char tabla7seg[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

unsigned char contador = 0;

void mostrarNumero(unsigned char num) {
    unsigned char decenas = num / 10;
    unsigned char unidades = num % 10;
    unsigned char i;

    for (i = 0; i < 20; i++) {
        PORTC = 0x01;
        PORTD = tabla7seg[decenas];
        __delay_ms(2);

        PORTC = 0x02;
        PORTD = tabla7seg[unidades];
        __delay_ms(2);
    }
}

void main(void) {
    TRISB = 0B00000111;
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0B00000111;

    TRISC = 0x00;
    TRISD = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    ANSEL = 0x00;
    ANSELH = 0x00;

    while (1) {
        if (BTN_SUMA == 0) {
            __delay_ms(150);
            if (contador < 99) contador++;
            else contador = 0;
        }

        if (BTN_RESTA == 0) {
            __delay_ms(150);
            if (contador > 0) contador--;
            else contador = 99;
        }

        if (BTN_RESET == 0) {
            __delay_ms(150);
            contador = 0;
        }

        mostrarNumero(contador);
    }
}
```
--------------------------------------------------------------------------------------------------------

## 4. Circuito Base

- VDD (pines 11, 32) → +5 V | VSS (pines 12, 31) → GND
- R1 = 1 kΩ entre MCLR (pin 1) y VDD
- Cristal 8 MHz en RA6 (pin 14) y RA7 (pin 13)
- Configuration bits: oscilador XT, MCLRE habilitado, WDT deshabilitado

---

## 5. Conclusiones

Las prácticas de entradas digitales permitieron comprender el manejo de pull-ups internas para evitar resistencias externas en los botones, así como la lectura de entradas combinada con la multiplexación de displays de 7 segmentos para construir un contador funcional con suma, resta y reset.
