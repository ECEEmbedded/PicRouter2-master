/* 
 * File:   ColorSensor.h
 * Author: Dhiraj
 *
 * 
 */

#ifndef COLORSENSOR_H
#define	COLORSENSOR_H

#define Clock_8MHz
#define Baud_9600


#define STATUS_LED PORTB.3


#define WRITE_sda() TRISB = TRISB & 0b10111111 //SDA must be output when writing
#define READ_sda()  TRISB = TRISB | 0b01000000 //SDA must be input when reading - don't forget the resistor on SDA!!

#define SCL PORTB.7
#define SDA PORTB.6

#define I2C_DELAY   1

#define ACK     1
#define NO_ACK  0

//#define DEVICE_WRITE    0x74 //Default ADJD-S371 I2C address - write
//#define DEVICE_READ     0xE9 //Default ADJD-S371 I2C address - read

#define COLOR_ADDRESS  0x74


#define CAP_RED         0x06
#define CAP_GREEN       0x07
#define CAP_BLUE        0x08
#define CAP_CLEAR       0x09

#define INT_RED_LO      0x0A
#define INT_RED_HI      0x0B
#define INT_GREEN_LO    0x0C
#define INT_GREEN_HI    0x0D
#define INT_BLUE_LO     0x0E
#define INT_BLUE_HI     0x0F
#define INT_CLEAR_LO    0x10
#define INT_CLEAR_HI    0x11

#define DATA_RED_LO     0x40
#define DATA_RED_HI     0x41
#define DATA_GREEN_LO   0x42
#define DATA_GREEN_HI   0x43
#define DATA_BLUE_LO    0x44
#define DATA_BLUE_HI    0x45
#define DATA_CLEAR_LO   0x46
#define DATA_CLEAR_HI   0x47

void boot_up(void);
void Color_init(void);

void ack_polling(unsigned char device_address);
void write_register(unsigned char register_name, unsigned char register_value);
unsigned char read_register(unsigned char register_name);
void color_read(void);


#endif	/* COLORSENSOR_H */

