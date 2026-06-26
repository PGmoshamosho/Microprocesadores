# Proyecto Final — Comunicaciones: Juego de Penales entre dos PIC

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887 (x2)  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, el proyecto se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó cada PIC.

Para esta práctica se seleccionó como protocolo de comunicación el **intercambio de señales digitales por líneas dedicadas** (handshaking por GPIO) entre dos microcontroladores PIC16F887, cada uno con su propio LCD, para implementar un juego de penales: un PIC controla al **jugador/pateador** ("B") y el otro controla al **portero** ("P"). Cada PIC corre su propio ciclo de juego y se comunican mediante señales digitales para indicar la fila del disparo, el momento del tiro, y el resultado (gol o atajada).

---

## 2. Desarrollo

### Software
- MPLAB IDE X
- Compilador XC8
- Proteus Design Suite

### Hardware
- 2x PIC16F887 (uno por jugador)
- 2x PICkit (ICSP)
- 2x Cristal 8 MHz + botón de reinicio
- 2x Resistencia pull-up 1 kΩ en MCLR
- 2x Display LCD 16x2 (PORTC: RS=RC2, EN=RC3, D4-D7=RC4-RC7)
- Joystick analógico en el PIC del Pateador (AN0=eje Y, AN1=eje X) + botón en RB0
- Joystick analógico en el PIC del Portero (AN0=eje Y)
- Líneas de comunicación digital entre ambos PIC (PORTB)
- Fuente 5 V (Arduino) por cada PIC

---

## 3. Descripción del Proyecto

| Rol | Componente | Puerto | Función |
|---|---|---|---|
| Pateador ("B") | Joystick (2 ejes) + botón + LCD | AN0+AN1 (RA0,RA1) + RB0 + PORTC | Mueve al jugador, elige fila/columna del disparo y dispara |
| Portero ("P") | Joystick (1 eje) + LCD | AN0 (RA0) + PORTC | Mueve al portero para intentar atajar el disparo |

### Señales de comunicación entre los dos PIC

| Señal | Pin en Pateador | Pin en Portero | Descripción |
|---|---|---|---|
| Fila del balón | RB2 (salida) | RB1 (entrada) | El Pateador indica en qué fila (0 o 1) va el disparo |
| Señal de tiro | RB3 (salida) | RB3 (entrada) | El Pateador indica el momento exacto del disparo |
| Atajada | RB4 (entrada) | RB4 (salida) | El Portero indica que detuvo el balón |
| Gol | RB5 (entrada) | RB5 (salida) | El Portero indica que no logró atajar |

El jugador (Pateador) mueve su personaje en el LCD usando un joystick: el eje Y selecciona la fila (arriba/abajo) y el eje X selecciona la posición del balón respecto al jugador. Al presionar el botón del joystick (RB0), se envía la fila del disparo al Portero (RB2) y se anima el balón recorriendo las columnas del LCD hasta llegar al borde, momento en el que se activa la señal de tiro (RB3). El Portero, mientras tanto, mueve su personaje verticalmente con su propio joystick (AN0) tratando de coincidir con la fila del disparo; al recibir la señal de tiro, compara su posición con la fila recibida y envía la señal correspondiente de atajada (RB4) o gol (RB5) de regreso al Pateador, mostrando el resultado en ambos displays LCD.

<img width="500" height="618" alt="image" src="https://github.com/user-attachments/assets/b101c366-0a22-471f-ab00-919537d397bc" />
Figura 1

<img width="1414" height="584" alt="image" src="https://github.com/user-attachments/assets/3637c87d-73e6-43b3-a9bc-9567ac0b5049" />

Figura 2



<img width="500" height="266" alt="image" src="https://github.com/user-attachments/assets/e2b4c5a2-3ec9-4f0c-b5d6-28bc3170dc63" />


Figura 3

--------------------------------------------------------------------------------------------------------
## Codigo — PIC del Pateador ("B")
```c
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lcdb.h"

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};

unsigned char fila_jugador = 0;
unsigned char col_jugador = 0;
unsigned char col_balon = 2;

unsigned char balon_negro[8] = {
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00000
};

unsigned char jugador[8] = {
    0b00100,
    0b01110,
    0b00100,
    0b01110,
    0b10101,
    0b00110,
    0b01001,
    0b10000
};

void ADC_Init(){
    ANSEL = 0x03; // RA0 y RA1 analogicos
    ANSELH = 0x00;

    ADCON0 = 0x81;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(unsigned char canal){
    ADCON0 = (canal << 2) | 0x01;
    __delay_ms(2);

    GO_nDONE = 1;
    while(GO_nDONE);

    return ((ADRESH << 8) + ADRESL);
}

void CrearCaracter(unsigned char pos, unsigned char dibujo[]){
    LCD_Cmd(0x40 + (pos * 8));

    for(unsigned char i = 0; i < 8; i++){
        LCD_putc(dibujo[i]);
    }

    LCD_Cmd(0x80);
}

void Leer_Joystick(){
    unsigned int adc_y;
    unsigned int adc_x;

    adc_y = ADC_Read(0); // RA0 = Y
    adc_x = ADC_Read(1); // RA1 = X

    if(adc_y < 400){
        fila_jugador = 0;
    }
    else if(adc_y > 600){
        fila_jugador = 1;
    }

    col_jugador = ((unsigned long)adc_x * 12) / 1023;
    col_balon = col_jugador + 2;

    if(col_balon > 14){
        col_balon = 14;
    }
}

void Mandar_Fila_Balon(unsigned char fila){
    if(fila == 0){
        PORTBbits.RB2 = 0;
    }
    else{
        PORTBbits.RB2 = 1;
    }
}

void Dibujar_Jugador_Balon(){
    LCD_Clear();

    LCD_Set_Cursor(fila_jugador, col_jugador);
    LCD_putc(1);

    LCD_Set_Cursor(fila_jugador, col_balon);
    LCD_putc(0);

    LCD_Set_Cursor(0,15);
    LCD_putc('|');

    LCD_Set_Cursor(1,15);
    LCD_putc('|');
}

void Reiniciar_Balon(){
    fila_jugador = 0;
    col_jugador = 0;
    col_balon = 2;

    PORTBbits.RB2 = 0;
    PORTBbits.RB3 = 0;

    LCD_Clear();

    LCD_Set_Cursor(fila_jugador, col_jugador);
    LCD_putc(1);

    LCD_Set_Cursor(fila_jugador, col_balon);
    LCD_putc(0);

    LCD_Set_Cursor(0,15);
    LCD_putc('|');

    LCD_Set_Cursor(1,15);
    LCD_putc('|');
}

void Mostrar_Gol(){
    LCD_Clear();

    LCD_Set_Cursor(0,0);
    LCD_putrs("GOOOO");

    LCD_Set_Cursor(1,0);
    LCD_putrs("Gol!");

    __delay_ms(10000);

    Reiniciar_Balon();
}

void Mostrar_Atajada(){
    LCD_Clear();

    LCD_Set_Cursor(0,0);
    LCD_putrs("ATAJADA!");

    LCD_Set_Cursor(1,0);
    LCD_putrs("Reinicio");

    __delay_ms(5000);

    Reiniciar_Balon();
}

void Esperar_Resultado(){
    unsigned int tiempo = 0;

    while(tiempo < 20000){

        if(PORTBbits.RB5 == 1){
            Mostrar_Gol();
            return;
        }

        if(PORTBbits.RB4 == 1){
            Mostrar_Atajada();
            return;
        }

        __delay_ms(100);
        tiempo += 100;
    }

    Reiniciar_Balon();
}

void Disparo(){
    unsigned char col;
    unsigned char fila_tiro;
    unsigned char col_tiro;

    fila_tiro = fila_jugador;
    col_tiro = col_balon;

    Mandar_Fila_Balon(fila_tiro);

    for(col = col_tiro; col <= 15; col++){

        Leer_Joystick();

        LCD_Clear();

        LCD_Set_Cursor(fila_jugador, col_jugador);
        LCD_putc(1);

        LCD_Set_Cursor(fila_tiro, col);
        LCD_putc(0);

        LCD_Set_Cursor(0,15);
        LCD_putc('|');

        LCD_Set_Cursor(1,15);
        LCD_putc('|');

        __delay_ms(70);
    }

    PORTBbits.RB3 = 1;
    __delay_ms(500);
    PORTBbits.RB3 = 0;

    Esperar_Resultado();
}

void main(void){
    ADC_Init();

    TRISAbits.TRISA0 = 1; // Joystick Y
    TRISAbits.TRISA1 = 1; // Joystick X

    TRISBbits.TRISB0 = 1; // Boton SW del joystick
    TRISBbits.TRISB2 = 0; // Manda fila
    TRISBbits.TRISB3 = 0; // Manda tiro
    TRISBbits.TRISB4 = 1; // Recibe atajada
    TRISBbits.TRISB5 = 1; // Recibe gol

    OPTION_REGbits.nRBPU = 0; // Pull-ups de PORTB activados
    WPUBbits.WPUB0 = 1; // Pull-up en RB0

    PORTBbits.RB2 = 0;
    PORTBbits.RB3 = 0;

    LCD_Init(lcd);

    CrearCaracter(0, balon_negro);
    CrearCaracter(1, jugador);

    LCD_Clear();

    while(1){
        Leer_Joystick();
        Mandar_Fila_Balon(fila_jugador);
        Dibujar_Jugador_Balon();

        // Boton del joystick presionado = 0
        if(PORTBbits.RB0 == 0){
            __delay_ms(50);

            if(PORTBbits.RB0 == 0){
                Disparo();

                while(PORTBbits.RB0 == 0);
                __delay_ms(300);
            }
        }

        __delay_ms(100);
    }
}
```
--------------------------------------------------------------------------------------------------------

## Codigo — PIC del Portero ("P")
```c
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lcdp.h"

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};

unsigned char fila_portero = 0;
unsigned char fila_balon = 0;

unsigned char balon_negro[8] = {
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00000
};

unsigned char portero[8] = {
    0b00100,
    0b01110,
    0b10101,
    0b00100,
    0b01110,
    0b10101,
    0b01010,
    0b10001
};

void ADC_Init(){
    ANSEL = 0x01;
    ANSELH = 0x00;

    ADCON0 = 0x81;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(){
    ADCON0 = 0x81;
    __delay_ms(2);

    GO_nDONE = 1;
    while(GO_nDONE);

    return ((ADRESH << 8) + ADRESL);
}

void CrearCaracter(unsigned char pos, unsigned char dibujo[]){
    LCD_Cmd(0x40 + (pos * 8));

    for(unsigned char i = 0; i < 8; i++){
        LCD_putc(dibujo[i]);
    }

    LCD_Cmd(0x80);
}

void Leer_Joystick_Portero(){
    unsigned int adc;

    adc = ADC_Read();

    if(adc < 400){
        fila_portero = 0;
    }
    else if(adc > 600){
        fila_portero = 1;
    }
}

void Leer_Fila_Balon(){
    if(PORTBbits.RB1 == 0){
        fila_balon = 0;
    }
    else{
        fila_balon = 1;
    }
}

void Dibujar_Portero(){
    LCD_Clear();

    LCD_Set_Cursor(0,15);
    LCD_putc('|');

    LCD_Set_Cursor(1,15);
    LCD_putc('|');

    LCD_Set_Cursor(fila_portero,14);
    LCD_putc(1);
}

void Reiniciar_Portero(){
    fila_portero = 0;
    fila_balon = 0;

    PORTBbits.RB4 = 0;
    PORTBbits.RB5 = 0;

    LCD_Clear();

    LCD_Set_Cursor(0,15);
    LCD_putc('|');

    LCD_Set_Cursor(1,15);
    LCD_putc('|');

    LCD_Set_Cursor(fila_portero,14);
    LCD_putc(1);
}

void Atajada_Portero(){
    PORTBbits.RB4 = 1;
    __delay_ms(500);
    PORTBbits.RB4 = 0;

    LCD_Clear();

    LCD_Set_Cursor(0,0);
    LCD_putrs("ATAJADA!");

    LCD_Set_Cursor(1,0);
    LCD_putrs("Reinicio");

    __delay_ms(5000);

    Reiniciar_Portero();
}

void Gol_Portero(){
    PORTBbits.RB5 = 1;
    __delay_ms(500);
    PORTBbits.RB5 = 0;

    LCD_Clear();

    LCD_Set_Cursor(0,0);
    LCD_putrs("OOOOOL");

    LCD_Set_Cursor(1,0);
    LCD_putrs("Gol!");

    __delay_ms(10000);

    Reiniciar_Portero();
}

void Recibir_Tiro(){
    unsigned char col;
    unsigned char fila_tiro;

    Leer_Fila_Balon();
    fila_tiro = fila_balon;

    for(col = 0; col <= 14; col++){

        Leer_Joystick_Portero();

        LCD_Clear();

        LCD_Set_Cursor(fila_tiro, col);
        LCD_putc(0);

        LCD_Set_Cursor(0,15);
        LCD_putc('|');

        LCD_Set_Cursor(1,15);
        LCD_putc('|');

        LCD_Set_Cursor(fila_portero,14);
        LCD_putc(1);

        __delay_ms(70);
    }

    Leer_Joystick_Portero();

    if(fila_tiro == fila_portero){
        Atajada_Portero();
    }
    else{
        Gol_Portero();
    }

    __delay_ms(300);
}

void main(void){
    ADC_Init();

    TRISAbits.TRISA0 = 1; // Joystick Y del portero

    TRISBbits.TRISB1 = 1; // Recibe fila del balon
    TRISBbits.TRISB3 = 1; // Recibe senal de tiro
    TRISBbits.TRISB4 = 0; // Manda atajada
    TRISBbits.TRISB5 = 0; // Manda gol

    PORTBbits.RB4 = 0;
    PORTBbits.RB5 = 0;

    LCD_Init(lcd);

    CrearCaracter(0, balon_negro);
    CrearCaracter(1, portero);

    LCD_Clear();

    while(1){
        Leer_Joystick_Portero();
        Dibujar_Portero();

        if(PORTBbits.RB3 == 1){
            Recibir_Tiro();
            __delay_ms(300);
        }

        __delay_ms(100);
    }
}
```
--------------------------------------------------------------------------------------------------------

## 4. Circuito Base (aplica a ambos PIC)

- VDD (pines 11, 32) → +5 V | VSS (pines 12, 31) → GND
- R1 = 1 kΩ entre MCLR (pin 1) y VDD
- Cristal 8 MHz en RA6 (pin 14) y RA7 (pin 13)
- Configuration bits: oscilador HS, MCLRE habilitado, WDT deshabilitado
- LCD: RS=RC2, EN=RC3, D4=RC4, D5=RC5, D6=RC6, D7=RC7
- GND compartido entre ambos PIC para que las señales de PORTB sean referencia común

---

## 5. Conclusiones

Este proyecto integró prácticamente todos los conceptos vistos en el curso —ADC, manejo de LCD con caracteres personalizados, lectura de botones, y comunicación entre microcontroladores— en una aplicación interactiva de dos jugadores. La comunicación entre ambos PIC se resolvió con líneas digitales dedicadas en lugar de UART, ya que la cantidad de información a transmitir (una fila de 1 bit, una señal de disparo, y dos señales de resultado) era pequeña y no justificaba la complejidad de un protocolo serial; cada señal tiene su propio pin con dirección fija (salida en un PIC, entrada en el otro), funcionando como un protocolo de handshaking simple. El reto principal fue sincronizar la animación del balón en ambos displays para que el momento del "tiro" coincidiera visualmente con la decisión del portero, resuelto haciendo que ambos PIC corran su propia animación de forma independiente pero arrancando a partir de la misma señal de disparo.
