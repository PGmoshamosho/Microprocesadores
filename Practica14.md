# Practica 14 — Servomotores (Actividad 1, Actividad 2)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el control de un servomotor mediante señales PWM generadas por software con retardos directos (Actividad 1) y mediante un Timer1 configurado como temporizador de microsegundos junto con lectura analógica de un potenciómetro (Actividad 2).

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
- Servomotor en RC0
- Potenciómetro en AN0 (RA0, Actividad 2)
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Act. 1 | Servomotor | RC0 | Barrido de -90° a +90° aproximado (izquierda/derecha) |
| Act. 2 | Servomotor + Potenciómetro | RC0 + AN0 (RA0) | Ángulo de giro controlado por potenciómetro |

### Actividad 1 — Barrido del servomotor (izquierda/derecha)
Se generó la señal PWM necesaria para un servomotor estándar mediante pulsos directos con `__delay_us`: un pulso de 600 µs aproxima la posición izquierda (-90°) y uno de 2400 µs la posición derecha (+90°), repitiendo cada pulso varias veces con un periodo de 20 ms para mantener al servo en esa posición.

<!-- ESPACIO PARA IMAGEN: Figura 1. Simulación de Actividad 1 -->

<!-- ESPACIO PARA IMAGEN: Figura 2. Actividad de Actividad 1 -->

<!-- ESPACIO PARA VIDEO: Video 1. Actividad 1 -->
--------------------------------------------------------------------------------------------------------
## Codigo
```c
#include <xc.h>

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

void Servo_Izquierda(){
    for(unsigned char i = 0; i < 25; i++){
        PORTCbits.RC0 = 1;
        __delay_us(600);
        PORTCbits.RC0 = 0;
        __delay_ms(20);
    }
}


void Servo_Derecha(){
    for(unsigned char i = 0; i < 25; i++){
        PORTCbits.RC0 = 1;
        __delay_us(2400);
        PORTCbits.RC0 = 0;
        __delay_ms(20);
    }
}

void main(void){
    ANSEL = 0x00;
    ANSELH = 0x00;

    TRISCbits.TRISC0 = 0;
    PORTCbits.RC0 = 0;

    while(1){
        Servo_Izquierda(); // -90 aprox
        __delay_ms(500);


        Servo_Derecha(); // +90 aprox
        __delay_ms(500);


    }
}
```
--------------------------------------------------------------------------------------------------------

### Actividad 2 — Ángulo de giro controlado por potenciómetro
Se utilizó la lectura promediada de un potenciómetro en AN0 para calcular el pulso correspondiente entre 500 µs y 2500 µs (rango completo del servo), generando la señal PWM mediante el Timer1 configurado como temporizador de precisión en microsegundos.

<!-- ESPACIO PARA IMAGEN: Figura 3. Simulación de Actividad 2 -->

<!-- ESPACIO PARA IMAGEN: Figura 4. Actividad de Actividad 2 -->

<!-- ESPACIO PARA VIDEO: Video 2. Actividad 2 -->
--------------------------------------------------------------------------------------------------------
## Codigo
```c
#include <xc.h>

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000

#define SERVO PORTCbits.RC0

#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500

void Timer1_Init(void);
void Delay_us_TMR1(unsigned int us);

void ADC_Init(void);
unsigned int ADC_Read(unsigned char canal);
unsigned int ADC_Read_Avg(unsigned char canal);

void Servo_Write(unsigned int pulso_us);
void Servo_From_ADC(unsigned int adc_value);

void main(void) {
    unsigned int pot;

    ADC_Init();
    Timer1_Init();

    TRISCbits.TRISC0 = 0;
    SERVO = 0;

    while(1) {
        pot = ADC_Read_Avg(0);
        Servo_From_ADC(pot);
    }
}

void ADC_Init(void) {
    ANSEL = 0x01;
    ANSELH = 0x00;

    TRISAbits.TRISA0 = 1;

    ADCON0 = 0x01;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(unsigned char canal) {
    ADCON0 &= 0b11000011;
    ADCON0 |= (canal << 2);

    __delay_us(50);

    GO_nDONE = 1;
    while(GO_nDONE);

    return (unsigned int)(((unsigned int)ADRESH << 8) | ADRESL);
}

unsigned int ADC_Read_Avg(unsigned char canal) {
    unsigned long suma = 0;
    unsigned char i;

    for(i = 0; i < 8; i++) {
        suma += ADC_Read(canal);
    }

    return (unsigned int)(suma / 8);
}

void Timer1_Init(void) {
    T1CON = 0b00110001;
}

void Delay_us_TMR1(unsigned int us) {
    unsigned int ticks;
    unsigned int carga;

    ticks = us / 4;

    if(ticks == 0) {
        ticks = 1;
    }

    carga = 65536 - ticks;

    TMR1H = carga >> 8;
    TMR1L = carga & 0xFF;

    TMR1IF = 0;
    TMR1ON = 1;

    while(TMR1IF == 0);

    TMR1ON = 0;
}

void Servo_Write(unsigned int pulso_us) {
    SERVO = 1;
    Delay_us_TMR1(pulso_us);

    SERVO = 0;
    Delay_us_TMR1(20000 - pulso_us);
}

void Servo_From_ADC(unsigned int adc_value) {
    unsigned int pulso;

    pulso = SERVO_MIN_US + (((unsigned long)adc_value * (SERVO_MAX_US - SERVO_MIN_US)) / 1023);

    Servo_Write(pulso);
}
```
--------------------------------------------------------------------------------------------------------

## 4. Circuito Base

- VDD (pines 11, 32) → +5 V | VSS (pines 12, 31) → GND
- R1 = 1 kΩ entre MCLR (pin 1) y VDD
- Cristal 8 MHz en RA6 (pin 14) y RA7 (pin 13)
- Configuration bits: oscilador HS, MCLRE habilitado, WDT deshabilitado
- Servomotor: señal de control en RC0
- ADC (Actividad 2): AN0 = RA0 (potenciómetro)

---

## 5. Conclusiones

La Actividad 1 mostró que es posible controlar un servomotor con pulsos generados directamente por software usando `__delay_us`, aunque esto bloquea por completo la ejecución del programa durante cada pulso. La Actividad 2 mejoró esta aproximación usando el Timer1 configurado en modo temporizador para generar los pulsos con mayor precisión, además de promediar 8 lecturas del ADC (`ADC_Read_Avg`) para suavizar el ruido de la lectura analógica y evitar que el servo "tiemble" por pequeñas variaciones en el voltaje del potenciómetro.
