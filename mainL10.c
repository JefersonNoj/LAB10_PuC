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
#define largo_msg 7

// VARIABLES -------------------------------------------------------------------
char mensaje[largo_msg] = {' ',' ',' ',' ',' ', 0x0D, 0x0A};
char string[] = "Hola mundo \n";
uint8_t indice = 0, bandera = 0;     
uint8_t old_valor = 0, caracter_cont = 0;

// PROTOTIPO DE FUNCIONES ------------------------------------------------------
void setup(void);
void imp_cadena(char arreglo[]);

// INTERRUPCIONES --------------------------------------------------------------
void __interrupt() isr (void){
    if(PIR1bits.RCIF){              //Verificar si hay datos recibidos
        bandera = 0;
        mensaje[caracter_cont] = RCREG;         //Guardar el valor recibido en el arreglo del mensaje
        PORTB = mensaje[caracter_cont];         //Mostrar el valor recibido en el PORTB
        caracter_cont++;                        // Aumentar contador de caracteres del arreglo
        if (caracter_cont > 4)                  
            caracter_cont = 0;                  // Reiniciar si es mayor a 4
    }
}

// CICLO PRINCIPAL -------------------------------------------------------------
void main(void){
    setup();
    while(1){
        __delay_ms(1000);
        imp_cadena(string);      // Ejecutar subrutina que muestra el mensaje
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
    RCSTAbits.RX9 = 0;          // Utilizar solo 8 bits para recepción 
    TXSTAbits.TXEN = 1;         // Habilitar transmisor
    RCSTAbits.CREN = 1;         // Habilitar receptor
    TXSTAbits.TX9 = 0;          // Utilizar solo 8 bits para transmición 
    
    // Configuración de interrupciones
    INTCONbits.GIE = 1;         // Habilitar interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitar interrupciones de perifericos
    PIE1bits.RCIE = 1;          // Habilitar Interrupciones de recepción
    
    return;
}

void imp_cadena(char arreglo[]){
    int cursor = 0;
    while(arreglo[cursor]!=0){            // Loop para imprimir el mensaje completo
        if (PIR1bits.TXIF){             // Esperar a que esté libre el TXREG para poder enviar por el serial
            TXREG = arreglo[cursor];    // Cargar el caracter a enviar
            cursor++;                   // Incrementar indice para enviar sigiente caracter
        }
    } 
    return;
}