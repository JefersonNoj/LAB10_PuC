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
#define largo_msg 9

// VARIABLES -------------------------------------------------------------------
char mensaje[largo_msg] = {'D','a','t','o',':',' ',' ', 0x0D, 0x0A};
uint8_t indice = 0;     
uint8_t old_valor = 0;

// PROTOTIPO DE FUNCIONES ------------------------------------------------------
void setup(void);

// INTERRUPCIONES --------------------------------------------------------------
void __interrupt() isr (void){
    /*if(PIR1bits.RCIF){              //Verificar si hay datos recibidos
        mensaje[6] = RCREG;         //Guardar el valor recibido en el arreglo del mensaje
        PORTB = mensaje[6];         //Mostrar el valor recibido en el PORTB
    }*/
}

// CICLO PRINCIPAL -------------------------------------------------------------
void main(void){
    setup();
    while(1){
        __delay_ms(500);
        if(PIR1bits.TXIF){
            TXREG = 0x4A;
        }
        /*indice = 0;                             // Reiniciamos indice para enviar todo el mensaje
        if (old_valor != mensaje[6]){           // Verificamos que el nuevo valor recibido en el serial 
                                                //   sea diferente al anterior, para imprimir solo 
            while(indice<largo_msg){              // Loop para imprimir el mensaje completo
                if (PIR1bits.TXIF){             // Esperamos a que est  libre el ?TXREG para poder enviar por el serial
                    TXREG = mensaje[indice];    // Cargamos caracter a enviar
                    indice++;                   // Incrementamos indice para enviarsigiente caracter
                }
            }
            old_valor = mensaje[6];             // Guardamos valor recibido para comparar en siguiente iteraci n?
                                                //   si el nuevo valor recibido es diferente al anterior. 
        }*/
    }
    return;
}

// CONFIGURACIONES -------------------------------------------------------------
void setup(void){
    ANSEL = 0;
    ANSELH = 0;                 // I/O digitales
    
    TRISB = 0;
    PORTB = 0;                  // PORTB como salida
    
    OSCCONbits.IRCF = 0b100;    // 1MHz
    OSCCONbits.SCS = 1;         // Oscilador interno

    //Configuración de comunicacion serial
    //SYNC = 0, BRGH = 1, BRG16 = 1, SPBRG=25 <- Valores de tabla 12-5
    TXSTAbits.SYNC = 0;         // Comunicación ascincrona (full-duplex)
    TXSTAbits.BRGH = 1;         // Baud rate de alta velocidad 
    BAUDCTLbits.BRG16 = 1;      // 16-bits para generar el baud rate
    
    SPBRG = 25;
    SPBRGH = 0;                 // Baud rate ~9600, error -> 0.16%
    
    RCSTAbits.SPEN = 1;         // Habilitar comunicación
    RCSTAbits.RX9 = 0;
    TXSTAbits.TXEN = 1;         // Habilitar transmisor
    /*RCSTAbits.CREN = 1;         // Habilitar receptor
    TXSTAbits.TX9 = 0;          // Utilizar solo 8 bits
    
    // Configuración de interrupciones
    INTCONbits.GIE = 1;         // Habilitar interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitar interrupciones de perifericos
    PIE1bits.RCIE = 1;          // Habilitar Interrupciones de recepción*/
    
    return;
}




