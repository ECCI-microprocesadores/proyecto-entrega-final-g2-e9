#ifndef LORA_H
#define LORA_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

// Definición de frecuencia del cristal
#define _XTAL_FREQ 16000000     // 16MHz

// Definiciones de pines SPI y LoRa
#define SPI_NSS                 LATAbits.LATA5    // NSS/CS
#define SPI_RESET               LATDbits.LATD0    // RESET
#define DIO0_PIN                PORTCbits.RC0     // Pin DIO0 para interrupciones

// Registros del SX1278
#define REG_FIFO                0x00
#define REG_OP_MODE             0x01
#define REG_FRF_MSB             0x06
#define REG_FRF_MID             0x07
#define REG_FRF_LSB             0x08
#define REG_PA_CONFIG           0x09
#define REG_OCP                 0x0B
#define REG_LNA                 0x0C
#define REG_FIFO_ADDR_PTR       0x0D
#define REG_FIFO_TX_BASE_ADDR   0x0E
#define REG_FIFO_RX_BASE_ADDR   0x0F
#define REG_IRQ_FLAGS           0x12
#define REG_MODEM_CONFIG_1      0x1D
#define REG_MODEM_CONFIG_2      0x1E
#define REG_PREAMBLE_MSB        0x20
#define REG_PREAMBLE_LSB        0x21
#define REG_PAYLOAD_LENGTH      0x22
#define REG_MODEM_CONFIG_3      0x26
#define REG_SYNC_WORD           0x39
#define REG_DIO_MAPPING_1       0x40
#define REG_VERSION             0x42
#define REG_PA_DAC              0x4D

// Valores de configuración para modos
#define MODE_SLEEP              0x00
#define MODE_STDBY              0x01
#define MODE_TX                 0x03
#define MODE_LONG_RANGE_MODE    0x80
#define IRQ_TX_DONE_MASK        0x08

// Estructura para el paquete del sensor
typedef struct {
    uint8_t deviceID;        // ID del dispositivo (identificador único)
    uint8_t sensorType;      // Tipo de sensor (0x01 = Reed switch)
    uint8_t sensorState;     // Estado del sensor (0 = cerrado, 1 = abierto)
    uint8_t counter;         // Contador de activaciones
    uint16_t timestamp;      // Timestamp simple (incrementa cada segundo)
} SensorPacket;

// Prototipos de funciones SPI
void initSPIHardware(void);
void resetModule(void);
uint8_t spiTransfer(uint8_t data);
void writeRegister(uint8_t addr, uint8_t value);
uint8_t readRegister(uint8_t addr);

// Prototipos de funciones LoRa
bool initLoRa(void);
bool sendSensorData(SensorPacket* packet);
void testSPICommunication(void);

#endif // LORA_H