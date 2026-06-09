# Reporte 2 — Matriz de LEDs 8x8 (P4, P5)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el control de una matriz de LEDs 8x8 mediante multiplexado, usando PORTB para las filas y PORTD para las columnas, desplegando una X estática y una secuencia de letras de los nombres del equipo (O, R, M, H).

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
- Matriz de LEDs 8x8
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| P4 | Matriz 8x8 LEDs | PORTB + PORTD | Patrón estático mostrando una X |
| P5 | Matriz 8x8 LEDs | PORTB + PORTD | Alternando entre letras O, R, M, H |

### P4 — Matriz 8x8 (X)
Se conectó la matriz usando PORTB para filas y PORTD para columnas. El código implementa multiplexado por filas: activa una fila a la vez y coloca el patrón de columnas correspondiente a la forma de una X. El ciclo corre suficientemente rápido para que la imagen se vea estática por persistencia visual.
<img width="981" height="582" alt="X" src="https://github.com/user-attachments/assets/c29b6112-23f6-41cf-9457-d002c76923be" />
Figura 7. Simulación de “X”

<img width="981" height="582" alt="imagen_2026-06-08_191920678" src="https://github.com/user-attachments/assets/5523eac5-292e-48ed-907a-da996a3be6ea" />
Figura 8. Actividad de “X”


<img width="981" height="582" alt="X" src="https://github.com/user-attachments/assets/f820b3f4-ba14-4bf3-91ff-233bc3eac812" />
Video 4. Simulacion de "X"

--------------------------------------------------------------------------------------------------------
Codigo

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

    TRISB = 0b00000000;
    TRISD = 0b00000000;

    PORTB = 0b11111111;
    PORTD = 0b00000000;

    while(1) {

        PORTB = 0b11111110;
        PORTD = 0b10000001;
        __delay_ms(2);

        PORTB = 0b11111101;
        PORTD = 0b01000010;
        __delay_ms(2);

        PORTB = 0b11111011;
        PORTD = 0b00100100;
        __delay_ms(2);

        PORTB = 0b11110111;
        PORTD = 0b00011000;
        __delay_ms(2);

        PORTB = 0b11101111;
        PORTD = 0b00011000;
        __delay_ms(2);

        PORTB = 0b11011111;
        PORTD = 0b00100100;
        __delay_ms(2);

        PORTB = 0b10111111;
        PORTD = 0b01000010;
        __delay_ms(2);

        PORTB = 0b01111111;
        PORTD = 0b10000001;
        __delay_ms(2);
    }
}
--------------------------------------------------------------------------------------------------------

### P5 — Matriz 8x8 (Letras O, R, M, H)
Mismo circuito que P4. El código almacena los bitmaps de las letras O, R, M y H en arreglos y alterna entre ellas con un retardo de aproximadamente 1 segundo, desplegando cada letra mediante el mismo esquema de multiplexado.
<img width="1003" height="633" alt="Letras" src="https://github.com/user-attachments/assets/ad6fb1a3-227b-4d62-b68e-8ea8e6664bf5" />
Figura 9. Simulación de Letras 

<img width="981" height="585" alt="imagen_2026-06-08_192141759" src="https://github.com/user-attachments/assets/0b56e925-d891-41d9-aa48-0436d27fe3a0" />
Figura 10. Actividad de Letras 

<img width="981" height="585" alt="Letras" src="https://github.com/user-attachments/assets/593da62d-d989-4c98-82e5-8ba9dc6b5c00" />

Video 5. Simulacion de Letras

--------------------------------------------------------------------------------------------------------
Codigo

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

--------------------------------------------------------------------------------------------------------
## 4. Circuito Base

- VDD (pines 11, 32) → +5 V | VSS (pines 12, 31) → GND
- R1 = 1 kΩ entre MCLR (pin 1) y VDD
- Cristal 8 MHz en RA6 (pin 14) y RA7 (pin 13)
- Configuration bits: oscilador HS, MCLRE habilitado, WDT deshabilitado

---

## 5. Conclusiones

El multiplexado de la matriz 8x8 fue la parte más desafiante, ya que requirió sincronizar correctamente filas y columnas a una velocidad suficiente para evitar parpadeo visible. El uso de arreglos para almacenar los bitmaps de cada letra resultó una solución clara y fácil de modificar para agregar más caracteres si se necesitara.






