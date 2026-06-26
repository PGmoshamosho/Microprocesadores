# Practica 13 — Motor DC (Actividad 1, Actividad 2)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el control de un motor DC: primero su velocidad de giro usando PWM por hardware sobre un driver basado en transistor, y después tanto la velocidad como el sentido de giro usando un puente H controlado con un segundo potenciómetro como selector de dirección.

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
- Motor DC + driver basado en transistor (Actividad 1) / puente H (Actividad 2)
- Potenciómetro(s) en AN0 (RA0) y AN1 (RA1, Actividad 2)
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Act. 1 | Motor DC + driver BJT | RC2 (PWM) + AN0 (RA0) | Velocidad de giro controlada por potenciómetro |
| Act. 2 | Motor DC + puente H | RC2 (PWM) + RC0/RC1 (dirección) + AN0+AN1 | Velocidad y sentido de giro |

### Actividad 1 — Velocidad de motor con driver basado en transistor
Se controló la velocidad de giro de un motor DC mediante un driver basado en un transistor, usando PWM por hardware (CCP1, RC2) cuyo ciclo de trabajo se ajusta con la lectura de un potenciómetro en AN0.

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

void ADC_Init(){
    ANSEL = 0x01;
    ANSELH = 0x00;

    ADCON0 = 0x81;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(){
    __delay_us(20);

    GO_nDONE = 1;
    while(GO_nDONE);

    return ((ADRESH << 8) + ADRESL);
}

void PWM_Init(){
    TRISCbits.TRISC2 = 0;

    PR2 = 255;
    CCP1CON = 0x0C;

    CCPR1L = 0;
    CCP1CONbits.DC1B = 0;

    T2CON = 0x04;
}

void PWM_Duty(unsigned int duty){
    if(duty > 1023){
        duty = 1023;
    }

    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B = duty & 0x03;
}

void main(void){
    unsigned int valor_adc;

    TRISAbits.TRISA0 = 1;

    ADC_Init();
    PWM_Init();

    while(1){
        valor_adc = ADC_Read();
        PWM_Duty(valor_adc);
        __delay_ms(20);
    }
}
```
--------------------------------------------------------------------------------------------------------

### Actividad 2 — Velocidad y sentido de giro con puente H
Se utilizó un puente H para controlar tanto la dirección de giro del motor (RC0/RC1 según el valor de un segundo potenciómetro en AN1) como su velocidad (PWM en RC2 controlado por el potenciómetro en AN0). Una zona intermedia en la lectura del segundo potenciómetro (entre 400 y 600) detiene el motor.

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

void ADC_Init(){
    ANSEL = 0x03;
    ANSELH = 0x00;

    ADCON0 = 0x81;
    ADCON1 = 0x80;
}

unsigned int ADC_Read(unsigned char canal){
    ADCON0 = (canal << 2) | 0x01;
    __delay_us(20);

    GO_nDONE = 1;
    while(GO_nDONE);

    return ((ADRESH << 8) + ADRESL);
}

void PWM_Init(){
    TRISCbits.TRISC2 = 0;

    PR2 = 255;
    CCP1CON = 0x0C;

    CCPR1L = 0;
    CCP1CONbits.DC1B = 0;

    T2CON = 0x04;
}

void PWM_Duty(unsigned int duty){
    if(duty > 1023){
        duty = 1023;
    }

    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B = duty & 0x03;
}

void main(void){
    unsigned int velocidad;
    unsigned int giro;

    ADC_Init();
    PWM_Init();

    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;

    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;

    PORTCbits.RC0 = 0;
    PORTCbits.RC1 = 0;

    while(1){
        velocidad = ADC_Read(0);
        giro = ADC_Read(1);

        PWM_Duty(velocidad);

        if(giro < 400){

            PORTCbits.RC0 = 1;
            PORTCbits.RC1 = 0;
        }
        else if(giro > 600){
            PORTCbits.RC0 = 0;
            PORTCbits.RC1 = 1;
        }
        else{
            PORTCbits.RC0 = 0;
            PORTCbits.RC1 = 0;
        }

        __delay_ms(20);
    }
}
```
--------------------------------------------------------------------------------------------------------

## 4. Circuito Base

- VDD (pines 11, 32) → +5 V | VSS (pines 12, 31) → GND
- R1 = 1 kΩ entre MCLR (pin 1) y VDD
- Cristal 8 MHz en RA6 (pin 14) y RA7 (pin 13)
- Configuration bits: oscilador HS, MCLRE habilitado, WDT deshabilitado
- ADC: AN0=RA0 (velocidad), AN1=RA1 (dirección, Actividad 2)
- PWM: RC2 (CCP1) | Dirección puente H: RC0, RC1 (Actividad 2)

---

## 5. Conclusiones

El control de velocidad con PWM funcionó de la misma forma que en la práctica de control de LEDs, demostrando que el mismo principio de ciclo de trabajo aplica igual para motores DC, solo que ahora a través de un driver capaz de manejar la corriente que requiere el motor. En la Actividad 2, el uso de un puente H permitió además invertir la polaridad aplicada al motor para cambiar su sentido de giro, usando una zona muerta central en la lectura del segundo potenciómetro para detener el motor de forma segura al transicionar entre direcciones.
