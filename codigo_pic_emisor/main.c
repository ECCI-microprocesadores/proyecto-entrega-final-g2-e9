#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "lora.h"

// Configuración de fusibles
#pragma config FOSC = INTIO67   // Oscilador interno
#pragma config WDTEN = OFF      // Watchdog timer deshabilitado
#pragma config PBADEN = OFF     // PORTB como I/O digital
#pragma config LVP = OFF        // Programación de bajo voltaje desactivada

// Definiciones de pines locales
#define LED                     LATAbits.LATA0    // LED de estado
#define LED_SENSOR              LATAbits.LATA1    // LED indicador de sensor
#define REED_SWITCH             PORTBbits.RB2     // Pin del sensor Reed switch

// Estados del sensor Reed
#define REED_OPEN               1    // Sensor abierto (sin imán)
#define REED_CLOSED             0    // Sensor cerrado (con imán)

// Variables globales
volatile uint8_t lastReedState = REED_OPEN;
volatile uint8_t currentReedState = REED_OPEN;
volatile bool sensorChanged = false;
volatile uint8_t activationCounter = 0;
volatile uint16_t systemTimestamp = 0;

// Prototipos de funciones locales
void initSystem(void);
void checkReedSwitch(void);
void debounceDelay(void);
void indicateSuccess(void);
void indicateError(void);
void indicateSensorActivation(void);
void updateTimestamp(void);

/**
 * Inicialización del sistema incluyendo sensor Reed
 */
void initSystem(void) {
    // Configurar oscilador interno a 16MHz
    OSCCON = 0x70;
    while(!OSCCONbits.IOFS);    // Esperar estabilización
    
    // Configurar pines como digitales
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    
    // Configurar pin CS/NSS (manual)
    TRISAbits.TRISA5 = 0;       // NSS como salida
    SPI_NSS = 1;                // CS inactivo en alto
    
    // Configurar pin DIO0 para interrupciones
    TRISCbits.TRISC0 = 1;       // DIO0 como entrada
    
    // Configurar LEDs
    TRISAbits.TRISA0 = 0;       // LED principal como salida
    TRISAbits.TRISA1 = 0;       // LED sensor como salida
    LED = 0;
    LED_SENSOR = 0;
    
    // Configurar pin de reset
    TRISDbits.TRISD0 = 0;       // RESET como salida
    SPI_RESET = 1;              // Inactivo en alto
    
    // Configurar sensor Reed switch
    TRISBbits.TRISB2 = 1;       // Reed switch como entrada
    
    // Habilitar pull-up interno para el sensor Reed
    INTCON2bits.nRBPU = 0;      // Habilitar pull-ups PORTB
    WPUBbits.WPUB2 = 1;         // Pull-up en RB2
    
    // Leer estado inicial del sensor Reed
    lastReedState = REED_SWITCH;
    currentReedState = lastReedState;
    
    // Inicializar módulo SPI hardware
    initSPIHardware();
}

/**
 * Verifica el estado del sensor Reed switch con debounce
 */
void checkReedSwitch(void) {
    currentReedState = REED_SWITCH;
    
    // Detectar cambio de estado
    if(currentReedState != lastReedState) {
        debounceDelay();  // Delay para debounce
        
        // Verificar que el cambio persiste
        if(REED_SWITCH == currentReedState) {
            lastReedState = currentReedState;
            sensorChanged = true;
            
            // Incrementar contador solo en activaciones (cerrado)
            if(currentReedState == REED_CLOSED) {
                activationCounter++;
                indicateSensorActivation();  // Indicación visual
            }
        }
    }
}

/**
 * Delay para debounce del sensor
 */
void debounceDelay(void) {
    __delay_ms(50);  // 50ms debounce
}

/**
 * Actualiza el timestamp del sistema (llamar cada segundo)
 */
void updateTimestamp(void) {
    systemTimestamp++;
}

/**
 * Indica activación del sensor con LED
 */
void indicateSensorActivation(void) {
    // Parpadeo rápido del LED del sensor
    for(uint8_t i = 0; i < 5; i++) {
        LED_SENSOR = 1;
        __delay_ms(100);
        LED_SENSOR = 0;
        __delay_ms(100);
    }
}

/**
 * Indica éxito con patrón LED (3 parpadeos)
 */
void indicateSuccess(void) {
    for(uint8_t i = 0; i < 3; i++) {
        LED = 1;
        __delay_ms(200);
        LED = 0;
        __delay_ms(200);
    }
}

/**
 * Indica error con patrón LED (SOS)
 */
void indicateError(void) {
    // Patrón SOS
    for(uint8_t i = 0; i < 3; i++) {
        LED = 1;
        __delay_ms(200);
        LED = 0;
        __delay_ms(200);
    }
    __delay_ms(400);
    
    for(uint8_t i = 0; i < 3; i++) {
        LED = 1;
        __delay_ms(600);
        LED = 0;
        __delay_ms(200);
    }
    __delay_ms(400);
    
    for(uint8_t i = 0; i < 3; i++) {
        LED = 1;
        __delay_ms(200);
        LED = 0;
        __delay_ms(200);
    }
}

/**
 * Función principal
 */
void main(void) {
    // Inicializar hardware
    initSystem();
    
    // Señal de inicio
    LED = 1;
    __delay_ms(100);
    LED = 0;
    __delay_ms(900);
    
    // Ejecutar diagnóstico de comunicación SPI
    testSPICommunication();
    __delay_ms(2000);
    
    // Inicializar módulo LoRa
    if(!initLoRa()) {
        while(1) {
            indicateError();
            __delay_ms(1000);
        }
    }
    
    // Indicar inicialización exitosa
    indicateSuccess();
    
    // Configurar paquete del sensor
    SensorPacket sensorPacket;
    sensorPacket.deviceID = 0x01;        // ID único del dispositivo
    sensorPacket.sensorType = 0x01;      // Tipo: Reed switch
    sensorPacket.sensorState = REED_OPEN;
    sensorPacket.counter = 0;
    sensorPacket.timestamp = 0;
    
    uint16_t timestampCounter = 0;
    
    // Bucle principal
    while(1) {
        // Verificar sensor Reed switch
        checkReedSwitch();
        
        // Si hubo cambio en el sensor, enviar datos
        if(sensorChanged) {
            // Actualizar paquete con estado actual
            sensorPacket.sensorState = currentReedState;
            sensorPacket.counter = activationCounter;
            sensorPacket.timestamp = systemTimestamp;
            
            // Enviar datos del sensor
            if(sendSensorData(&sensorPacket)) {
                indicateSuccess();
            } else {
                indicateError();
            }
            
            sensorChanged = false;  // Reset flag
        }
        
        // Actualizar timestamp cada segundo aproximadamente
        timestampCounter++;
        if(timestampCounter >= 1000) {  // Aprox. 1000ms con delays
            updateTimestamp();
            timestampCounter = 0;
            
            // Parpadeo lento para indicar que el sistema está vivo
            LED = 1;
            __delay_ms(10);
            LED = 0;
        }
        
        __delay_ms(1);  // Delay base del bucle
    }
}