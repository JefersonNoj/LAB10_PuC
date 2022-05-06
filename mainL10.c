/* 
 * File:   mainL10.c
 * Author: Jeferson Noj
 *
 * Created on 3 de mayo de 2022, 09:38 AM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>

// CONSTANTES ------------------------------------------------------------------
#define _XTAL_FREQ 1000000

// VARIABLES -------------------------------------------------------------------
//char string[] = "Hola mundo \r\n";
char op1[] = "\r\n1. LEER POT \r\n";    // Opción 1 del menú que se muestra en la terminal
char op2[] = "2. ENVIAR ASCII \r\n";    // Opción 2 del menú que se muestra en la terminal
char serial[6] = {0x0D, 0x0A, 0x40, 0x0D, 0x0A, 0x00};      // Arreglo para mostrar el resultado de las operaciones
uint8_t indice = 0, menuFlag = 1, POT_valor = 0;
uint8_t op1Flag = 0, op2Flag = 0, enviarFlag = 0;

// PROTOTIPO DE FUNCIONES ------------------------------------------------------
void setup(void);
void imp_cadena(char arreglo[]);

// INTERRUPCIONES --------------------------------------------------------------
void __interrupt() isr (void){
    
    if(PIR1bits.ADIF){              // Verificar si ocurrió interrupción del ADC
        if(ADCON0bits.CHS == 0)     // Verificar que se esta leyendo el AN0
            POT_valor = ADRESH;     // Guardar el resultado de la conversión
        PIR1bits.ADIF = 0;          // Limpiar bandera de interrupción del ADC
    }
    
    else if(PIR1bits.RCIF){         // Verificar si hay datos recibidos
        switch(RCREG){              // Evaluar el RCREG para determinar la opción que fue seleccionada
        case 49:                    // Si se escogió la opción 1
            if (op2Flag){           // Evaluar si anteriormente se había seleccionado "Enviar ASCII"
                enviarFlag = 1;     // Activar bandera para mostrar el caracter que se envió
                serial[2] = RCREG;  // Guardar RCREG en el arreglo que muestra el reultado 
            }
            else{
                serial[2] = POT_valor;  // Si no se había seleccionado "Enviar ASCII"
                op1Flag = 1;            //  activar bandera para ejecutar la opción 1
            }
            break;
        case 50:                    // Si se escogió la opción 2
            if (op2Flag){           // Evaluar si anteriormente se había seleccionado "Enviar ASCII"
                enviarFlag = 1;     // Activar bandera para mostrar el caracter que se envió
                serial[2] = RCREG;  // Guardar RCREG en el arreglo que muestra el reultado 
            }
            else{
                op2Flag = 1;        // Activar bandera para ejecutar la opción 2 y recibir el ASCII
            }
            break;
        default:
            menuFlag = 0;           // Apagar bandera del menu para no mostrarlo siempre
            op1Flag = 0;            // Apagar bandera para ejecutar la opción 1
            if (op2Flag){           // Evaluar si se había seleccionado "Enviar ASCII"
                enviarFlag = 1;     //  si fue seleccionado, activar bandera para mostrar el caracter que se envió
                serial[2] = RCREG;  // Guardar RCREG en el arreglo que muestra el resultado
            }
        }
    }
    return;
}

// CICLO PRINCIPAL -------------------------------------------------------------
void main(void){
    setup();
    while(1){
        if(ADCON0bits.GO == 0)    // Verificar que no hay proceso de conversión del ADC
            ADCON0bits.GO = 1;    // Inicar proceso de conversión
        
        if(menuFlag){             // Evaluar bandera del menú
            imp_cadena(op1);      // Imprimir/mostrar la primera opción del menú
            imp_cadena(op2);      // Imprimir/mostrar la segunda opción del menú
            menuFlag = 0;         // Apagar bandera del menú para no mostrarlo siempre
        }
        if (op1Flag){             // Evaluar bandera de "Leer pot"
            imp_cadena(serial);   // Si está activa, imprimir/mostrar el valor leído del pot
            op1Flag = 0;          // Apagar bandera de "Leer pot"
            menuFlag = 1;         // Activar bandera del menú para mostrarlo de nuevo (reiniciar pregunta)
        }
        while(op2Flag){           // Evaluar bandera de "Enviar ASCII"
            if(enviarFlag){             // Evaluar bandera para determinar si ya se envió el ASCII
                PORTB = RCREG;          // Si ya se ha enviado el caracter, mostrarlo en el PORTB
                imp_cadena(serial);     // Imprimir/mostrar 
                enviarFlag = 0;         
                op2Flag = 0;            // Apagar bandera de "Enviar ASCII"
                menuFlag = 1;           // Activar bandera del menú para mostrarlo de nuevo
            }
        }
    }
    return;
}

// CONFIGURACIONES -------------------------------------------------------------
void setup(void){
    ANSEL = 0b1;                // AN0 como entraga analógica
    ANSELH = 0;                 // I/O digitales
    
    TRISA = 0b1;                // AN0 como entrada
    PORTA = 0;                  // Limpiar PORTA
    TRISB = 0;
    PORTB = 0;                  // PORTB como salida
    
    OSCCONbits.IRCF = 0b100;    // 1MHz
    OSCCONbits.SCS = 1;         // Oscilador interno

    // Configuración del ACD
    ADCON0bits.ADCS = 0b01;     // FOSC/8
    ADCON1bits.VCFG0 = 0;       // VDD
    ADCON1bits.VCFG1 = 0;       // VSS
    ADCON0bits.CHS = 0b0000;    // Selecionar AN0
    ADCON1bits.ADFM = 0;        // Justificar a la izquierda
    ADCON0bits.ADON = 1;        // Habilitar modulo ADC
    __delay_us(40);             // Tiempo de muestreo
    
    //Configuración de comunicacion serial
    //SYNC = 0, BRGH = 1, BRG16 = 1, SPBRG=25 <- Valores de tabla 12-5
    TXSTAbits.SYNC = 0;         // Comunicación ascincrona (full-duplex)
    TXSTAbits.BRGH = 1;         // Baud rate de alta velocidad 
    BAUDCTLbits.BRG16 = 1;      // 16-bits para generar el baud rate
    
    SPBRG = 25;
    SPBRGH = 0;                 // Baud rate ~9600, error -> 0.16%
    
    RCSTAbits.SPEN = 1;         // Habilitar comunicación
    RCSTAbits.RX9 = 0;          // Utilizar solo 8 bits para recepción 
    TXSTAbits.TXEN = 1;         // Habilitar transmisor
    RCSTAbits.CREN = 1;         // Habilitar receptor
    TXSTAbits.TX9 = 0;          // Utilizar solo 8 bits para transmición 
    
    // Configuración de interrupciones
    INTCONbits.GIE = 1;         // Habilitar interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitar interrupciones de perifericos
    PIE1bits.RCIE = 1;          // Habilitar Interrupciones de recepción
    PIE1bits.ADIE = 1;          // Habilitar interrupción del ADC
    PIR1bits.ADIF = 0;          // Limpiar bandera del ADC
    
    return;
}

void imp_cadena(char arreglo[]){
    int cursor = 0;                     // Declarar e inicar variable local para recorrer el arreglo
    while(arreglo[cursor]!=0){          // Loop para imprimir el arreglo completo
        if (PIR1bits.TXIF){             // Esperar a que esté libre el TXREG para poder enviar por el serial
            TXREG = arreglo[cursor];    // Cargar el caracter a enviar
            cursor++;                   // Incrementar indice para enviar sigiente caracter
        }
    } 
    return;
}