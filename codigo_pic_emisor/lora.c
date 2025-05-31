#include "lora.h"

/**
 * Inicialización del módulo SPI hardware
 */
void initSPIHardware(void) {
    // Configurar pines del módulo SPI1
    // RC3/SCK1 - Clock (salida en modo Master)
    // RC4/SDI1 - MISO (entrada)
    // RC5/SDO1 - MOSI (salida)
    
    TRISCbits.TRISC3 = 0;       // SCK como salida
    TRISCbits.TRISC4 = 1;       // SDI (MISO) como entrada
    TRISCbits.TRISC5 = 0;       // SDO (MOSI) como salida
    
    // Deshabilitar el módulo SPI antes de configurar
    SSP1CON1bits.SSPEN = 0;
    
    // Limpiar el buffer de recepción leyendo SSP1BUF
    uint8_t dummy = SSP1BUF;
    
    // Configurar SSP1STAT
    // SMP = 0: Muestreo en el medio del tiempo de salida de datos (importante para SX1278)
    // CKE = 1: Transmitir en transición de activo a idle
    // Para modo SPI 0 (CPOL=0, CPHA=0): CKP=0, CKE=1
    SSP1STAT = 0x40;  // SMP=0, CKE=1
    
    // Configurar SSP1CON1
    // SSPEN = 1: Habilitar módulo SPI
    // CKP = 0: Clock idle en bajo (CPOL=0)
    // SSPM[3:0] = 0010: SPI Master, clock = Fosc/64 (250kHz @ 16MHz)
    SSP1CON1 = 0x02;  // CKP=0, Master Fosc/64
    
    // Habilitar el módulo SPI
    SSP1CON1bits.SSPEN = 1;
    
    // Limpiar flag de interrupción
    PIR1bits.SSP1IF = 0;
    
    // Limpiar cualquier dato pendiente
    dummy = SSP1BUF;
}

/**
 * Reset completo del módulo SX1278
 */
void resetModule(void) {
    SPI_RESET = 0;              // Activar reset
    __delay_ms(10);
    SPI_RESET = 1;              // Desactivar reset
    __delay_ms(10);
    
    // Ciclos de CS para despertar el módulo
    for(uint8_t i = 0; i < 3; i++) {
        SPI_NSS = 0;
        __delay_ms(1);
        SPI_NSS = 1;
        __delay_ms(1);
    }
    
    __delay_ms(10);
}

/**
 * Transferencia SPI usando hardware
 */
uint8_t spiTransfer(uint8_t data) {
    uint8_t received;
    
    // Verificar que no hay transmisión en curso
    while(SSP1STATbits.BF);     // Esperar si el buffer está lleno
    
    // Limpiar flag de interrupción
    PIR1bits.SSP1IF = 0;
    
    // Escribir dato al buffer de transmisión
    SSP1BUF = data;
    
    // Esperar a que se complete la transmisión
    while(!PIR1bits.SSP1IF);
    
    // Leer y retornar el dato recibido
    received = SSP1BUF;
    
    return received;
}

/**
 * Escribir un registro
 */
void writeRegister(uint8_t addr, uint8_t value) {
    // Asegurar que CS está alto antes de comenzar
    SPI_NSS = 1;
    __delay_us(10);
    
    SPI_NSS = 0;                // Activar CS
    __delay_us(5);              // Setup time para CS
    
    spiTransfer(addr | 0x80);   // Dirección con bit de escritura (bit 7 = 1)
    __delay_us(2);              // Pequeño delay entre bytes
    spiTransfer(value);         // Valor a escribir
    
    __delay_us(5);              // Hold time para CS
    SPI_NSS = 1;                // Desactivar CS
    __delay_us(10);             // Delay entre transacciones
}

/**
 * Leer un registro
 */
uint8_t readRegister(uint8_t addr) {
    uint8_t value;
    
    // Asegurar que CS está alto antes de comenzar
    SPI_NSS = 1;
    __delay_us(10);
    
    SPI_NSS = 0;                // Activar CS
    __delay_us(5);              // Setup time para CS
    
    spiTransfer(addr & 0x7F);   // Dirección con bit de lectura (bit 7 = 0)
    __delay_us(2);              // Pequeño delay entre bytes
    value = spiTransfer(0x00);  // Enviar dummy para recibir dato
    
    __delay_us(5);              // Hold time para CS
    SPI_NSS = 1;                // Desactivar CS
    __delay_us(10);             // Delay entre transacciones
    
    return value;
}

/**
 * Inicializar el módulo LoRa
 */
bool initLoRa(void) {
    uint8_t version;
    uint8_t retry = 0;
    
    // Reset del módulo
    resetModule();
    
    // Intentar leer la versión varias veces
    for(retry = 0; retry < 5; retry++) {
        version = readRegister(REG_VERSION);
        
        // Debug: mostrar versión leída con parpadeo
        if(version == 0x12) {
            // Versión correcta - parpadeo corto
            LATAbits.LATA0 = 1;
            __delay_ms(50);
            LATAbits.LATA0 = 0;
            __delay_ms(50);
            break;
        } else {
            // Versión incorrecta - parpadeo largo
            LATAbits.LATA0 = 1;
            __delay_ms(500);
            LATAbits.LATA0 = 0;
            __delay_ms(500);
            
            // Reintentar con reset
            resetModule();
            __delay_ms(100);
        }
    }
    
    // Si no se pudo leer la versión correcta
    if(version != 0x12) {
        return false;
    }
    
    // Poner en modo sleep primero
    writeRegister(REG_OP_MODE, MODE_SLEEP);
    __delay_ms(15);
    
    // Verificar que entró en modo sleep
    uint8_t mode = readRegister(REG_OP_MODE);
    if((mode & 0x07) != MODE_SLEEP) {
        return false;
    }
    
    // Activar modo LoRa
    writeRegister(REG_OP_MODE, MODE_SLEEP | MODE_LONG_RANGE_MODE);
    __delay_ms(15);
    
    // Verificar que el modo LoRa está activo
    mode = readRegister(REG_OP_MODE);
    if(!(mode & MODE_LONG_RANGE_MODE)) {
        return false;
    }
    
    // Configurar frecuencia 433MHz
    writeRegister(REG_FRF_MSB, 0x6C);
    __delay_ms(5);
    writeRegister(REG_FRF_MID, 0x80);
    __delay_ms(5);
    writeRegister(REG_FRF_LSB, 0x00);
    __delay_ms(5);
    
    // Configurar FIFO
    writeRegister(REG_FIFO_TX_BASE_ADDR, 0x00);
    writeRegister(REG_FIFO_RX_BASE_ADDR, 0x00);
    
    // Configuración de potencia
    writeRegister(REG_PA_CONFIG, 0x8F);     // PA_BOOST, max power
    __delay_ms(5);
    writeRegister(REG_PA_DAC, 0x84);        // PA DAC enable
    __delay_ms(5);
    writeRegister(REG_OCP, 0x2B);           // Over current protection
    __delay_ms(5);
    writeRegister(REG_LNA, 0x23);           // LNA gain
    
    // Configuración del módem LoRa
    writeRegister(REG_MODEM_CONFIG_1, 0x72);  // BW=125kHz, CR=4/5, explicit header
    __delay_ms(5);
    writeRegister(REG_MODEM_CONFIG_2, 0x74);  // SF=7, normal TX mode, CRC on
    __delay_ms(5);
    writeRegister(REG_MODEM_CONFIG_3, 0x04);  // Low data rate optimize off
    
    // Preámbulo
    writeRegister(REG_PREAMBLE_MSB, 0x00);
    writeRegister(REG_PREAMBLE_LSB, 0x08);    // 8 símbolos de preámbulo
    
    // Sync word
    writeRegister(REG_SYNC_WORD, 0x12);       // Sync word público de LoRa
    
    // DIO mapping para TxDone
    writeRegister(REG_DIO_MAPPING_1, 0x00);   // DIO0 = TxDone
    
    // Cambiar a modo standby
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    __delay_ms(15);
    
    // Verificar que está en standby
    mode = readRegister(REG_OP_MODE);
    if((mode & 0x07) != MODE_STDBY) {
        return false;
    }
    
    return true;
}

/**
 * Envía datos del sensor vía LoRa
 */
bool sendSensorData(SensorPacket* packet) {
    // Cambiar a modo standby
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    __delay_ms(10);
    
    // Configurar puntero FIFO
    writeRegister(REG_FIFO_ADDR_PTR, 0x00);
    
    // Escribir paquete de sensor en FIFO
    writeRegister(REG_FIFO, packet->deviceID);
    writeRegister(REG_FIFO, packet->sensorType);
    writeRegister(REG_FIFO, packet->sensorState);
    writeRegister(REG_FIFO, packet->counter);
    writeRegister(REG_FIFO, (packet->timestamp >> 8) & 0xFF);  // MSB timestamp
    writeRegister(REG_FIFO, packet->timestamp & 0xFF);         // LSB timestamp
    
    // Establecer longitud del payload (6 bytes)
    writeRegister(REG_PAYLOAD_LENGTH, 0x06);
    
    // Iniciar transmisión
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
    
    // Esperar transmisión completa
    uint16_t timeout = 0;
    while(timeout < 1000) {
        uint8_t irqFlags = readRegister(REG_IRQ_FLAGS);
        
        if(irqFlags & IRQ_TX_DONE_MASK) {
            // Limpiar flag
            writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
            
            // Volver a standby
            writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
            return true;
        }
        
        __delay_ms(1);
        timeout++;
    }
    
    // Timeout
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
    return false;
}

/**
 * Función de diagnóstico para probar la comunicación SPI
 */
void testSPICommunication(void) {
    uint8_t testValue;
    uint8_t i;
    
    // Test 1: Leer registro de versión múltiples veces
    for(i = 0; i < 3; i++) {
        testValue = readRegister(REG_VERSION);
        
        // Indicar el valor leído con parpadeos
        // 1 parpadeo = 0x10, 2 parpadeos = 0x12 (correcto), 3 parpadeos = otro
        if(testValue == 0x12) {
            // Correcto - 2 parpadeos cortos
            for(uint8_t j = 0; j < 2; j++) {
                LATAbits.LATA1 = 1;
                __delay_ms(100);
                LATAbits.LATA1 = 0;
                __delay_ms(100);
            }
        } else if(testValue == 0x00) {
            // Sin respuesta - 1 parpadeo largo
            LATAbits.LATA1 = 1;
            __delay_ms(500);
            LATAbits.LATA1 = 0;
        } else {
            // Valor incorrecto - 3 parpadeos rápidos
            for(uint8_t j = 0; j < 3; j++) {
                LATAbits.LATA1 = 1;
                __delay_ms(50);
                LATAbits.LATA1 = 0;
                __delay_ms(50);
            }
        }
        
        __delay_ms(1000);
    }
    
    // Test 2: Escribir y leer un registro de prueba
    // Usar el registro de sincronización que es R/W
    uint8_t originalSync = readRegister(REG_SYNC_WORD);
    __delay_ms(10);
    
    // Escribir valor de prueba
    writeRegister(REG_SYNC_WORD, 0xAA);
    __delay_ms(10);
    
    // Leer de vuelta
    testValue = readRegister(REG_SYNC_WORD);
    
    if(testValue == 0xAA) {
        // Escritura/lectura exitosa - parpadeo rápido
        for(uint8_t j = 0; j < 5; j++) {
            LATAbits.LATA0 = 1;
            __delay_ms(50);
            LATAbits.LATA0 = 0;
            __delay_ms(50);
        }
    } else {
        // Fallo en escritura/lectura - indicar error con patrón SOS
        // Patrón SOS
        for(uint8_t k = 0; k < 3; k++) {
            LATAbits.LATA0 = 1;
            __delay_ms(200);
            LATAbits.LATA0 = 0;
            __delay_ms(200);
        }
        __delay_ms(400);
        
        for(uint8_t k = 0; k < 3; k++) {
            LATAbits.LATA0 = 1;
            __delay_ms(600);
            LATAbits.LATA0 = 0;
            __delay_ms(200);
        }
        __delay_ms(400);
        
        for(uint8_t k = 0; k < 3; k++) {
            LATAbits.LATA0 = 1;
            __delay_ms(200);
            LATAbits.LATA0 = 0;
            __delay_ms(200);
        }
    }
    
    // Restaurar valor original
    writeRegister(REG_SYNC_WORD, originalSync);
}