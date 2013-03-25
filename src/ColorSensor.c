#include "maindefs.h"
#include "ColorSensor.h"
#include "i2cMaster.h"
#include "messages.h"

unsigned char temp_buffer[4];

void boot_up(void)
{
    //OSCCON = 0b.0111.0000; //Setup internal oscillator for 8MHz
    //while(OSCCON.2 == 0); //Wait for frequency to stabilize

    //Setup Ports
    //ANSEL = 0b.0000.0000; //Turn off A/D

    PORTA = 0b00000000;
    TRISA = 0b00000000;

    PORTB = 0b00010000;
    TRISB = 0b00000100;   //0 = Output, 1 = Input RX on RB2

    //Setup the hardware UART module
    //=============================================================
    SPBRG = 51; //8MHz for 9600 inital communication baud rate

    TXSTA = 0b00100100; //8-bit asych mode, high speed uart enabled
    RCSTA = 0b10010000; //Serial port enable, 8-bit asych continous receive mode
    //=============================================================

}

void Color_init(void)
{
    write_register(CAP_RED, 0x05);
    write_register(CAP_GREEN, 0x05);
    write_register(CAP_BLUE, 0x05);
    write_register(CAP_CLEAR, 0x05);

    write_register(INT_RED_LO, 0xC4);
    write_register(INT_RED_HI, 0x09);
    write_register(INT_GREEN_LO, 0xC4);
    write_register(INT_GREEN_HI, 0x09);
    write_register(INT_BLUE_LO, 0xC4);
    write_register(INT_BLUE_HI, 0x09);
    write_register(INT_CLEAR_LO, 0xC4);
    write_register(INT_CLEAR_HI, 0x09);
}

void write_register(unsigned char register_name, unsigned char register_value)
{
    temp_buffer[0] = register_name;
    temp_buffer[1] = register_value;
    
    i2c_master_send(MSGT_I2C_DATA, 2, temp_buffer);
    //Return nothing
}

void read_register(unsigned char register_name)
{
    i2c_master_request_reg(COLOR_ADDRESS, register_name, 1);
}

void color_read(void)
{
//    unsigned char response;
//    unsigned char redL, redH, greenL, greenH,  blueL, blueH, clearL, clearH;
//
//    write_register(0x00, 0b00000001); //Get sensor reading
//
//    while(1)
//    {
//        response = read_register(0x00);
//        if (response == 0) break;
//    }
//
//    //Red
//    redL = read_register(DATA_RED_LO);
//    redH = read_register(DATA_RED_HI);
//
//    //Green
//    greenL = read_register(DATA_GREEN_LO);
//    greenH = read_register(DATA_GREEN_HI);
//
//    //Blue
//    blueL = read_register(DATA_BLUE_LO);
//    blueH = read_register(DATA_BLUE_HI);
//
//    //Clear
//    clearL = read_register(DATA_CLEAR_LO);
//    clearH = read_register(DATA_CLEAR_HI);

}