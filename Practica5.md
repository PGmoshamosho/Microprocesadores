# Reporte 5 — Multiplexación e Interrupciones (Clase A, B, C)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren multiplexación de displays de 7 segmentos combinada con interrupciones externas: un contador 0-9 con parpadeo de LED al interrumpir, un contador de 4 dígitos multiplexados (0-9999) y una variante con cambio de dirección de conteo mediante interrupción.

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
- Displays de 7 segmentos multiplexados
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Clase A | Display 7 seg. | PORTD (segmentos) + PORTC (interrupción/control) | Patrón de 10 dígitos con interrupción externa |
| Clase B | Display 7 seg. multiplexado | PORTC (segmentos) + PORTD (selección) | Contador con interrupción y parpadeo de LED |
| Clase C | 4 displays 7 seg. multiplexados | PORTD (segmentos) + PORTC (selección) | Contador 0-9999 multiplexado |

### Clase A — Patrón de dígitos con interrupción externa
Se mostró una secuencia de dígitos de 7 segmentos en PORTD, controlada por interrupción externa (INT en RB0) para activar el parpadeo de un LED en RC0.

<img width="917" height="515" alt="image" src="https://github.com/user-attachments/assets/15e73102-bc70-402b-bbc0-6dcb506386c0" />
Figura 1



--------------------------------------------------------------------------------------------------------
## Codigo
```c
#include <xc.h> // Biblioteca principal del compilador XC8

//=============================================================================
// CONFIGURACIÓN DE BITS DE CONFIGURACIÓN (FUSES)
//=============================================================================

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

//=============================================================================
// DEFINICIONES
//=============================================================================

#define _XTAL_FREQ 8000000
#define LED PORTCbits.RC0

unsigned char patron[10]={
    0x3F,
    0x6,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x7,
    0x7F,
    0x67,
};

void blink_led(){
    for(int i = 0; i < 4; i++){
        LED = 1;
        __delay_ms(500);
        LED = 0;
        __delay_ms(500);
    }
}

void main(void){
    ANSEL = 0;
    ANSELH = 0;
    OPTION_REG = OPTION_REG & 0b01111111;

    TRISC = 0;
    TRISD = 0;
    TRISB = 0xFF;

    PORTC = 0;
    PORTD = 0;

    unsigned char count = 0;

    GIE = 1;  // ACTIVAR TODAS LAS INTERRUPCIONES
    INTE = 1;
    INTEDG = 0;

    while(1){
        PORTD = patron[count];
        count = (count + 1)%10;
        __delay_ms(500);
    }
}

void __interrupt() ISR(void){
    if(INTF){
        GIE = 0;       // DESACTIVAR TODAS LAS INTERRUPCIONES
        blink_led();   // ejecutar las funciones de la interrupcion
        GIE = 1;
        INTF = 0;
    }
}
```
--------------------------------------------------------------------------------------------------------

### Clase B — Contador con interrupción externa y parpadeo de LED
Se realizó un contador de 0 a 99 multiplexado en dos displays de 7 segmentos (PORTC para segmentos, PORTD para selección de display), donde una interrupción externa detiene el conteo y hace parpadear un LED antes de continuar.

--------------------------------------------------------------------------------------------------------
## Codigo
```c
#include <xc.h> // Biblioteca principal del compilador XC8

//=============================================================================
// CONFIGURACIÓN DE BITS DE CONFIGURACIÓN (FUSES)
//=============================================================================

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

//=============================================================================
// DEFINICIONES
//=============================================================================

#define _XTAL_FREQ 8000000
#define LED PORTCbits.RC0

unsigned char patron[10]={
    0x3F,
    0x6,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x7,
    0x7F,
    0x67,
};

int num = 0;
unsigned char direccion = 1;
unsigned char cambio = 0;

void mostrar_numero(int numero){
    int millar = numero / 1000;
    int centena = (numero / 100) % 10;
    int decena = (numero / 10) % 10;
    int unidad = numero % 10;

    PORTC = 0b11111110;
    PORTD = patron[millar];
    __delay_ms(1);

    PORTC = 0b11111101;
    PORTD = patron[centena];
    __delay_ms(1);

    PORTC = 0b11111011;
    PORTD = patron[decena];
    __delay_ms(1);

    PORTC = 0b11110111;
    PORTD = patron[unidad];
    __delay_ms(1);
}

void main(void){
    ANSEL = 0;
    ANSELH = 0;
    OPTION_REG = OPTION_REG & 0b01111111;

    TRISD = 0;
    TRISC = 0;
    TRISB = 0xFF;

    PORTD = 0;
    PORTC = 0x0F;

    INTF = 0;
    GIE = 1;
    INTE = 1;
    INTEDG = 0;

    while(1){
        for(int i = 0; i < 15; i++){
            mostrar_numero(num);
        }

        if(cambio == 1){
            direccion = !direccion;
            cambio = 0;

            for(int j = 0; j < 20; j++){
                mostrar_numero(num);
            }

            INTF = 0;
            INTE = 1;
        }

        if(direccion == 1){
            num++;

            if(num == 10000){
                num = 0;
            }
        }
        else{
            num--;

            if(num < 0){
                num = 9999;
            }
        }
    }
}

void __interrupt() ISR(void){
    if(INTF){
        INTE = 0;
        cambio = 1;
        INTF = 0;
    }
}
```
--------------------------------------------------------------------------------------------------------

### Clase C — Contador 0-9999 con 4 displays multiplexados
Se realizó un contador de 0 a 9999 utilizando 4 displays de 7 segmentos multiplexados, activando un display a la vez en PORTC y colocando el patrón correspondiente en PORTD.

<img width="720" height="1280" alt="image" src="https://github.com/user-attachments/assets/d3714925-327b-432b-95d1-d3eca930e064" />

Figura 2

<img width="720" height="1280" alt="image" src="https://github.com/user-attachments/assets/201866ed-31f7-42e3-ad19-7a2b7d5cf53f" />

Figura 3

<img width="977" height="606" alt="image" src="https://github.com/user-attachments/assets/4548c15b-46c8-42ba-9840-08d38f3547b2" />
Figura 4

--------------------------------------------------------------------------------------------------------
## Codigo
```c
#include <xc.h> // Biblioteca principal del compilador XC8

//=============================================================================
// CONFIGURACIÓN DE BITS DE CONFIGURACIÓN (FUSES)
//=============================================================================

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

//=============================================================================
// DEFINICIONES
//=============================================================================

#define _XTAL_FREQ 8000000

unsigned char patron[10]={
    0x3F,
    0x6,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x7,
    0x7F,
    0x67,
};

void main(void){
    TRISD = 0;
    TRISC = 0;

    PORTD = 0;
    PORTC = 0x0F;

    int num = 0;

    while(1){
        int millar = num / 1000;
        int centena = (num / 100) % 10;
        int decena = (num / 10) % 10;
        int unidad = num % 10;

        for(int i = 0; i < 30; i++){

            PORTC = 0b11111110; // Display 1
            PORTD = patron[millar];
            __delay_ms(1);

            PORTC = 0b11111101; // Display 2
            PORTD = patron[centena];
            __delay_ms(1);

            PORTC = 0b11111011; // Display 3
            PORTD = patron[decena];
            __delay_ms(1);

            PORTC = 0b11110111; // Display 4
            PORTD = patron[unidad];
            __delay_ms(1);
        }

        num++;

        if(num == 10000){
            num = 0;
        }
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

Las prácticas de multiplexación e interrupciones permitieron comprender cómo manejar varios displays de 7 segmentos con pocos pines, alternando rápidamente entre ellos, y cómo responder a eventos externos mediante interrupciones sin bloquear el conteo principal. El control de banderas (`INTF`, `cambio`) fue clave para evitar conflictos entre la rutina de interrupción y el ciclo principal del programa.
