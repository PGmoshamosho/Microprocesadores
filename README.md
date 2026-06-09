

# Reporte 1 — LEDs Individuales (P1, P2, P3)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el control de LEDs individuales conectados al PORTD: parpadeo de 4 LEDs, efecto de caminata con 6 LEDs y conteo binario de 6 bits sobre 6 LEDs.

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
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| P1 | LEDs individuales | PORTD | Control de 4 LEDs parpadeando |
| P2 | LEDs individuales | PORTD | Control de 6 LEDs haciendo la caminata |
| P3 | LEDs individuales | PORTD | Conteo en binario de 6 bits con 6 LEDs |

### P1 — LEDs individuales (Parpadeo)
Se conectaron 4 LEDs al PORTD con resistencias de 330 Ω. El código configura `TRISD = 0x00` y en el bucle principal asigna `0x0F` a PORTD para encender los 4 LEDs, espera un `delay_ms`, luego asigna `0x00` para apagarlos y espera otro `delay_ms`, repitiendo indefinidamente.

<img width="863" height="490" alt="Leds1" src="https://github.com/user-attachments/assets/859a3450-04f1-4e56-b121-8bbbbdd3e552" />

Figura 1. Simulación de Parpadeo 

<img width="1600" height="893" alt="Leds_1" src="https://github.com/user-attachments/assets/cc7ed641-3302-4bd0-8aed-5c5be60b3f61" />
Figura 2. Actividad de Parpadeo 

### P2 — LEDs individuales (Caminata)
Se conectaron 6 LEDs al PORTD con resistencias de 330 Ω. El código ejecuta un bucle que enciende un LED a la vez de forma secuencial (`0b000001 → 0b000010 → 0b000100...` y regresa), creando el efecto de caminata con `delay_ms` entre cada estado.

<img width="963" height="603" alt="Leds2" src="https://github.com/user-attachments/assets/d3bf208b-9929-4d02-9ae0-20afa1adecf0" />
Figura 3. Simulación de Caminata

<img width="1600" height="902" alt="led2" src="https://github.com/user-attachments/assets/8d241dda-b045-4cc0-ad3f-bdadf39a7eed" />
Figura 4. Actividad de Caminata


### P3 — LEDs individuales (Conteo binario)
Mismo circuito que P2. El código incrementa una variable de 0 a 63 (`0b000000` a `0b111111`) asignándola directamente a PORTD, representando cada número en binario sobre los 6 LEDs.

<img width="922" height="612" alt="Leds3" src="https://github.com/user-attachments/assets/5d4246fd-a5dd-4cbd-93b3-bf6a7390ea97" />
Figura 5. Simulación de Contador

<img width="1600" height="1017" alt="led3" src="https://github.com/user-attachments/assets/f7e51532-cab7-4c36-91be-8533e9f0d2f1" />
Figura 6. Actividad de Contador


---

## 4. Circuito Base

- VDD (pines 11, 32) → +5 V | VSS (pines 12, 31) → GND
- R1 = 1 kΩ entre MCLR (pin 1) y VDD
- Cristal 8 MHz en RA6 (pin 14) y RA7 (pin 13)
- Configuration bits: oscilador HS, MCLRE habilitado, WDT deshabilitado

---

## 5. Conclusiones

Las tres prácticas con LEDs individuales permitieron comprender el manejo básico de los registros TRISD y PORTD, así como el uso de delays para generar efectos visuales. Las resistencias de 330 Ω resultaron adecuadas para los LEDs con VDD = 5 V, manteniéndose dentro de los límites de corriente del puerto.
```
