# Reporte 12 — Teclado Matricial (Actividad 1, Actividad 2)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el manejo de un teclado matricial 4x4 conectado al PORTD: primero mostrando en el LCD el carácter de la tecla presionada, y después implementando una calculadora básica capaz de sumar, restar, multiplicar y dividir dos números ingresados desde el teclado.

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
- Teclado matricial 4x4 en PORTD (RD0-RD3 filas como salida, RD4-RD7 columnas como entrada)
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Act. 1 | Teclado 4x4 + LCD | PORTD (teclado) + PORTC (LCD) | Mostrar en LCD la tecla presionada |
| Act. 2 | Teclado 4x4 + LCD | PORTD (teclado) + PORTC (LCD) | Calculadora básica (+, −, ×, ÷) |

### Actividad 1 — Mostrar en el LCD la tecla presionada
Se conectó un teclado matricial 4x4 al PORTD (filas RD0-RD3 como salida, columnas RD4-RD7 como entrada con verificación por doble lectura para anti-rebote) y se mostró en el LCD el carácter correspondiente a la tecla presionada.

<img width="1049" height="724" alt="image" src="https://github.com/user-attachments/assets/52fc5c73-32e4-4d68-b05e-a7f91433acf4" />
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

LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};

char Leer_Teclado_Raw(){
    TRISD = 0xF0;

    PORTD = 0b00001110;
    __delay_ms(5);

    if(PORTDbits.RD4 == 0) return '1';
    if(PORTDbits.RD5 == 0) return '2';
    if(PORTDbits.RD6 == 0) return '3';
    if(PORTDbits.RD7 == 0) return 'A';

    PORTD = 0b00001101;
    __delay_ms(5);

    if(PORTDbits.RD4 == 0) return '4';
    if(PORTDbits.RD5 == 0) return '5';
    if(PORTDbits.RD6 == 0) return '6';
    if(PORTDbits.RD7 == 0) return 'B';

    PORTD = 0b00001011;
    __delay_ms(5);

    if(PORTDbits.RD4 == 0) return '7';
    if(PORTDbits.RD5 == 0) return '8';
    if(PORTDbits.RD6 == 0) return '9';
    if(PORTDbits.RD7 == 0) return 'C';

    PORTD = 0b00000111;
    __delay_ms(5);

    if(PORTDbits.RD4 == 0) return '*';
    if(PORTDbits.RD5 == 0) return '0';
    if(PORTDbits.RD6 == 0) return '#';
    if(PORTDbits.RD7 == 0) return 'D';

    return 0;
}

char Leer_Teclado(){
    char tecla1, tecla2;

    tecla1 = Leer_Teclado_Raw();

    if(tecla1 != 0){
        __delay_ms(50);
        tecla2 = Leer_Teclado_Raw();

        if(tecla1 == tecla2){
            while(Leer_Teclado_Raw() != 0);
            __delay_ms(50);
            return tecla1;
        }
    }

    return 0;
}

void Mostrar_Tecla(char tecla){
    LCD_Clear();

    LCD_Set_Cursor(0,0);
    LCD_putrs("Tecla:");

    LCD_Set_Cursor(1,0);
    LCD_putc(tecla);
}

void main(void){
    char tecla;

    ANSEL = 0x00;
    ANSELH = 0x00;

    TRISD = 0xF0;
    PORTD = 0xFF;

    LCD_Init(lcd);
    LCD_Clear();

    LCD_Set_Cursor(0,0);
    LCD_putrs("Presiona tecla");

    while(1){
        tecla = Leer_Teclado();

        if(tecla != 0){
            Mostrar_Tecla(tecla);
        }
    }
}
```
--------------------------------------------------------------------------------------------------------

### Actividad 2 — Calculadora básica (suma, resta, multiplicación, división)
Se implementó una calculadora básica: se ingresa el primer número, se selecciona la operación con `A` (suma), `B` (resta), `C` (multiplicación) o `D` (división), se ingresa el segundo número y con `#` se muestra el resultado en el LCD. La tecla `*` reinicia la operación en cualquier momento.

<img width="1071" height="722" alt="image" src="https://github.com/user-attachments/assets/2b8e4756-c639-43e8-ac51-4970a9b431f7" />
Figura 2

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

LCD lcd = {&PORTC, 2, 3, 4, 5, 6, 7};

char Leer_Teclado_Raw(){
    TRISD = 0xF0;

    PORTD = 0b00001110;
    __delay_ms(5);
    if(PORTDbits.RD4 == 0) return '1';
    if(PORTDbits.RD5 == 0) return '2';
    if(PORTDbits.RD6 == 0) return '3';
    if(PORTDbits.RD7 == 0) return 'A';

    PORTD = 0b00001101;
    __delay_ms(5);
    if(PORTDbits.RD4 == 0) return '4';
    if(PORTDbits.RD5 == 0) return '5';
    if(PORTDbits.RD6 == 0) return '6';
    if(PORTDbits.RD7 == 0) return 'B';

    PORTD = 0b00001011;
    __delay_ms(5);
    if(PORTDbits.RD4 == 0) return '7';
    if(PORTDbits.RD5 == 0) return '8';
    if(PORTDbits.RD6 == 0) return '9';
    if(PORTDbits.RD7 == 0) return 'C';

    PORTD = 0b00000111;
    __delay_ms(5);
    if(PORTDbits.RD4 == 0) return '*';
    if(PORTDbits.RD5 == 0) return '0';
    if(PORTDbits.RD6 == 0) return '#';
    if(PORTDbits.RD7 == 0) return 'D';

    return 0;
}

char Leer_Teclado(){
    char tecla1, tecla2;

    tecla1 = Leer_Teclado_Raw();

    if(tecla1 != 0){
        __delay_ms(50);
        tecla2 = Leer_Teclado_Raw();

        if(tecla1 == tecla2){
            while(Leer_Teclado_Raw() != 0);
            __delay_ms(50);
            return tecla1;
        }
    }

    return 0;
}

void Pantalla_Inicio(){
    LCD_Clear();
    LCD_Set_Cursor(0,0);
    LCD_putrs("Calculadora:");
    LCD_Set_Cursor(1,0);
}

void main(void){
    char tecla;
    char operador = 0;
    char buffer[16];

    long num1 = 0;
    long num2 = 0;
    long resultado = 0;
    long decimal = 0;
    long entero = 0;
    long decimales = 0;

    unsigned char estado = 1;
    unsigned char resultado_mostrado = 0;

    ANSEL = 0x00;
    ANSELH = 0x00;

    TRISD = 0xF0;
    PORTD = 0xFF;

    LCD_Init(lcd);
    Pantalla_Inicio();

    while(1){
        tecla = Leer_Teclado();

        if(tecla != 0){

            if(tecla >= '0' && tecla <= '9'){

                if(resultado_mostrado == 1){
                    num1 = 0;
                    num2 = 0;
                    resultado = 0;
                    operador = 0;
                    estado = 1;
                    resultado_mostrado = 0;
                    Pantalla_Inicio();
                }

                LCD_putc(tecla);

                if(estado == 1){
                    num1 = (num1 * 10) + (tecla - '0');
                }
                else if(estado == 2){
                    num2 = (num2 * 10) + (tecla - '0');
                }
            }

            else if(tecla == 'A'){
                if(resultado_mostrado == 0){
                    operador = '+';
                    estado = 2;
                    LCD_putc('+');
                }
            }

            else if(tecla == 'B'){
                if(resultado_mostrado == 0){
                    operador = '-';
                    estado = 2;
                    LCD_putc('-');
                }
            }

            else if(tecla == 'C'){
                if(resultado_mostrado == 0){
                    operador = '*';
                    estado = 2;
                    LCD_putc('*');
                }
            }

            else if(tecla == 'D'){
                if(resultado_mostrado == 0){
                    operador = '/';
                    estado = 2;
                    LCD_putc('/');
                }
            }

            else if(tecla == '#'){
                LCD_Clear();

                LCD_Set_Cursor(0,0);
                LCD_putrs("Resultado:");

                LCD_Set_Cursor(1,0);

                if(operador == '+'){
                    resultado = num1 + num2;
                    sprintf(buffer, "%ld+%ld=%ld", num1, num2, resultado);
                    LCD_putrs(buffer);
                }

                else if(operador == '-'){
                    resultado = num1 - num2;
                    sprintf(buffer, "%ld-%ld=%ld", num1, num2, resultado);
                    LCD_putrs(buffer);
                }

                else if(operador == '*'){
                    resultado = num1 * num2;
                    sprintf(buffer, "%ld*%ld=%ld", num1, num2, resultado);
                    LCD_putrs(buffer);
                }

                else if(operador == '/'){
                    if(num2 == 0){
                        LCD_putrs("Error div 0");
                    }
                    else{
                        decimal = (num1 * 100) / num2;
                        entero = decimal / 100;
                        decimales = decimal % 100;

                        sprintf(buffer, "%ld/%ld=%ld.%02ld", num1, num2, entero, decimales);
                        LCD_putrs(buffer);
                    }
                }

                else{
                    LCD_putrs("Error");
                }

                resultado_mostrado = 1;
            }

            else if(tecla == '*'){
                num1 = 0;
                num2 = 0;
                resultado = 0;
                operador = 0;
                estado = 1;
                resultado_mostrado = 0;
                Pantalla_Inicio();
            }
        }
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
- Teclado 4x4: filas RD0-RD3 (salida), columnas RD4-RD7 (entrada)

---

## 5. Conclusiones

El escaneo del teclado matricial se hizo activando una fila a la vez (poniéndola en bajo) y revisando qué columna detecta el nivel bajo, repitiendo el proceso para las 4 filas. La doble lectura con un pequeño retardo entre ambas (`Leer_Teclado_Raw` llamado dos veces) sirvió como anti-rebote, asegurando que la tecla detectada sea estable antes de aceptarla. La calculadora de la Actividad 2 mostró cómo manejar una máquina de estados simple (`estado` 1 y 2) para distinguir si el dígito ingresado corresponde al primer o segundo operando, junto con el manejo de la división con decimales usando aritmética entera escalada por 100.
