# Reporte 3 — Display 7 Segmentos (P6, P7)

**Materia:** Microprocesadores | **Microcontrolador:** PIC16F887  
**Alumnos:** Abraham Gamez Gonzalez 655549 | Omar Balcázar Pineda 652911

---

## 1. Introducción

El PIC16F887 es un microcontrolador de 8 bits de la familia PIC16 de Microchip Technology. Tiene 40 pines en encapsulado DIP-40, arquitectura RISC con 35 instrucciones, 14,208 bytes de memoria Flash, 368 bytes de RAM y 256 bytes de EEPROM. Sus cinco puertos de E/S (PORTA a PORTE) permiten conectar distintos periféricos, y además trae integrados timers, módulos CCP, USART, SPI, I2C y un ADC de 10 bits.

Para programarlo se usó MPLAB IDE con el compilador XC8, escribiendo el código en C y configurando los registros TRIS para definir si los pines son entrada o salida, junto con los configuration bits para ajustar cosas como el oscilador y el watchdog timer. Una vez compilado, el archivo .hex se cargó al PIC físico mediante un PICkit conectado por ICSP.

Antes de armar el circuito real, cada práctica se probó primero en Proteus, lo que permitió verificar que el código funcionara bien sin arriesgar componentes. Ya validado en simulación, se armó en protoboard y se programó el PIC.

Las prácticas de este reporte cubren el uso de un display de 7 segmentos de cátodo común conectado al PORTD, implementando un conteo decimal del 0 al 9 y un conteo hexadecimal completo del 0 al F.

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
- Display de 7 segmentos cátodo común
- Fuente 5 V (Arduino)

---

## 3. Prácticas

| Práctica | Componente | Puerto | Descripción |
|---|---|---|---|
| P6 | Display 7 seg. | PORTD | Conteo decimal de 0 a 9 |
| P7 | Display 7 seg. | PORTD | Conteo hexadecimal de 0 a F |

### P6 — Display 7 segmentos (Conteo 0–9)
Se conectó un display de 7 segmentos de cátodo común al PORTD. El código tiene un arreglo con los valores hexadecimales correspondientes a cada dígito del 0 al 9 y los asigna en secuencia a PORTD con un retardo de ~500 ms entre cada uno.

<img width="1031" height="627" alt="0-9" src="https://github.com/user-attachments/assets/208a5473-0065-406a-ac03-80e92b90eca1" />
Figura 11. Simulación de Conteo 0-9

<img width="1031" height="627" alt="0-9" src="https://github.com/user-attachments/assets/c3657730-021c-4d39-b10a-f5dc705459ce" />
Figura 12. Actividad de Conteo 0-9  




<img width="1031" height="627" alt="0-9" src="https://github.com/user-attachments/assets/2672488e-cd4c-4ebe-99d1-17b85e1f7ae0" />
Video 6 Simulacion conteo 0-9





### P7 — Display 7 segmentos (Conteo 0–F)
Igual que P6 pero el arreglo se extiende hasta la F, cubriendo los 16 valores hexadecimales (0x00–0x0F). Permite observar la representación en 7 segmentos de todos los dígitos hexadecimales, incluyendo las letras A, B, C, D, E y F.
<img width="1031" height="627" alt="0-F" src="https://github.com/user-attachments/assets/74401011-61a1-4961-a9ae-7457839dd014" />
Figura 13. Simulación de Conteo 0-F

<img width="1031" height="627" alt="0-F" src="https://github.com/user-attachments/assets/432c48b0-fb0a-4e3b-bb03-b0943bfa3dc9" />
Figura 14. Actividad de Conteo 0-F

<img width="1031" height="627" alt="display" src="https://github.com/user-attachments/assets/3196bdea-6fbb-4e7e-9e16-237cad29666e" />
Video 7 Simulacion de conteo 0-F

---

## 4. Circuito Base

- VDD (pines 11, 32) → +5 V | VSS (pines 12, 31) → GND
- R1 = 1 kΩ entre MCLR (pin 1) y VDD
- Cristal 8 MHz en RA6 (pin 14) y RA7 (pin 13)
- Configuration bits: oscilador HS, MCLRE habilitado, WDT deshabilitado

---

## 5. Conclusiones

El display de 7 segmentos resultó sencillo de manejar una vez definidos correctamente los valores de cada dígito en el arreglo. La diferencia entre P6 y P7 fue mínima en código pero permitió observar cómo el PIC puede representar valores hexadecimales de forma directa. Proteus fue útil para verificar que los segmentos correctos se encendieran para cada valor antes de probar en físico.
