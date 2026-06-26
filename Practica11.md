# Reporte 11 — PWM por Hardware y PWM por Software (Actividad 1, Actividad 2)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el control de intensidad de LEDs mediante PWM: primero usando el módulo CCP1 (PWM por hardware) controlado por un potenciómetro, y después combinando ese mismo PWM por hardware con un segundo PWM generado completamente por software usando el Timer1 para alternar el LED a la frecuencia deseada.

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
- LED en RC2 (CCP1/PWM hardware)
- LED en RD0 (PWM software, Actividad 2)
- Potenciómetro(s) en AN0 (RA0) y AN1 (RA1, Actividad 2)
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| Act. 1 | LED + Potenciómetro | RC2 (PWM) + AN0 (RA0) | Intensidad de 1 LED con PWM por hardware |
| Act. 2 | 2 LEDs + 2 Potenciómetros | RC2 (PWM HW) + RD0 (PWM SW) + AN0+AN1 | Intensidad de 2 LEDs (PWM hardware y software) |

### Actividad 1 — Intensidad de LED con potenciómetro (PWM por hardware)
Se utilizó el módulo CCP1 del PIC16F887 en modo PWM para controlar el brillo de un LED conectado a RC2. La lectura de un potenciómetro en AN0 define el ciclo de trabajo (duty cycle) mediante el Timer2 como base de tiempo.

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

### Actividad 2 — Intensidad de 2 LEDs (PWM hardware + software)
Se controló un LED con PWM por hardware (CCP1, RC2) usando un potenciómetro en AN0, y un segundo LED con PWM generado por software mediante el Timer1: una interrupción periódica cuenta ciclos de 0 a 99 y enciende/apaga el LED en RD0 según si el contador es menor al valor de duty calculado a partir de un potenciómetro en AN1.

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

volatile unsigned char duty_led2 = 0;
volatile unsigned char contador_pwm = 0;

void ADC_Init(){
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

void PWM_Hardware_Init(){
    TRISCbits.TRISC2 = 0;

    PR2 = 255;
    CCP1CON = 0x0C;

    CCPR1L = 0;
    CCP1CONbits.DC1B = 0;

    T2CON = 0x04;
}

void PWM_Hardware_Duty(unsigned int duty){
    if(duty > 1023){
        duty = 1023;
    }

    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B = duty & 0x03;
}

void Timer1_Init(){
    T1CON = 0x01;

    TMR1H = 0xFF;
    TMR1L = 0x38;

    TMR1IF = 0;
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;
}

void __interrupt() ISR(){
    if(TMR1IF){
        TMR1H = 0xFF;
        TMR1L = 0x38;

        contador_pwm++;

        if(contador_pwm >= 100){
            contador_pwm = 0;
        }

        if(contador_pwm < duty_led2){
            PORTDbits.RD0 = 1;
        }
        else{
            PORTDbits.RD0 = 0;
        }

        TMR1IF = 0;
    }
}

void main(void){
    unsigned int pot1;
    unsigned int pot2;

    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;

    TRISDbits.TRISD0 = 0;
    PORTDbits.RD0 = 0;

    ADC_Init();
    PWM_Hardware_Init();
    Timer1_Init();

    while(1){
        pot1 = ADC_Read(0);
        pot2 = ADC_Read(1);

        PWM_Hardware_Duty(pot1);

        duty_led2 = (pot2 * 100) / 1023;

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
- ADC: AN0=RA0 (Actividad 1 y 2), AN1=RA1 (Actividad 2)
- PWM hardware: RC2 (CCP1) | PWM software: RD0 (Actividad 2)

---

## 5. Conclusiones

El PWM por hardware (módulo CCP1) resultó mucho más sencillo de implementar, ya que el propio periférico del PIC genera la señal sin intervención del programa una vez configurado el periodo (PR2) y el duty cycle (CCPR1L + DC1B). El PWM por software, en cambio, requirió generar manualmente la señal mediante una interrupción periódica del Timer1 que incrementa un contador de 0 a 99 y compara contra el valor de duty deseado, simulando el comportamiento de un PWM real. Esta comparación práctica mostró las ventajas de los periféricos dedicados frente a las soluciones puramente programadas, que consumen más tiempo de CPU y son más sensibles a la precisión de los cálculos de temporización.
