#include "maindefs.h"
#include <stdio.h>
#include "timer0_thread.h"

// This is a "logical" thread that processes messages from TIMER0
// It is not a "real" thread because there is only the single main thread
// of execution on the PIC because we are not using an RTOS.

static int sensorToPoll;
static unsigned char colorSensorMsgCount;

void init_timer0_lthread(timer0_thread_struct *tptr){
#ifdef __MOTOR2680
    //Set up RA0-RA3 as Digital Input Pins
    PORTA = 0x00;
    ADCON1 = 0xF;
    TRISA = 0xCF;
#endif

    sensorToPoll = COLOR;
    sonarMsgCount = 0;
}

int timer0_lthread(timer0_thread_struct *tptr, int msgtype, int length, unsigned char *msgbuffer) {
    //unsigned int *msgval;
    //msgval = (unsigned int *) msgbuffer;

#ifdef __MASTER2680
    switch(sensorToPoll){
        case COLOR:
        {
            i2c_configure_master(COLOR_SENSOR_ADDR);
            //Do what you gotta do to poll the sonar
            sonarMsgCount++;
            unsigned char fakeData[I2C_MSG_SIZE];
            fakeData[0] = SONAR_MSG_TYPE;
            fakeData[1] = sonarMsgCount;
            fakeData[2] = 0;
            fakeData[3] = 0;
            fakeData[4] = 0;
            fakeData[5] = 0;
            fakeData[6] = 0;
            fakeData[7] = 0;
            //ToMainHigh_sendmsg(4,MSGT_I2C_MASTER_RECV_COMPLETE, fakeData);
            sensorToPoll = IR;
            break;
        }
        case IR:
        {
            i2c_configure_master(IR_SENSOR_ADDR);
            //FromMainLow_sendmsg(1,MSGT_I2C_RQST, (void*) msgbuffer);
            unsigned char fakeData[I2C_MSG_SIZE];
            fakeData[0] = CAMERA_MSG_TYPE;
            fakeData[1] = sonarMsgCount;
            fakeData[2] = 0;
            fakeData[3] = 0;
            fakeData[4] = 0;
            fakeData[5] = 0;
            fakeData[6] = 0;
            fakeData[7] = 0;
            //ToMainHigh_sendmsg(4,MSGT_I2C_MASTER_RECV_COMPLETE, fakeData);
            sensorToPoll = ENCODERS;
            break;
        }
        case ENCODERS:
            i2c_configure_master(ENCODERS_ADDR);
            FromMainLow_sendmsg(1,MSGT_I2C_RQST, (void*) msgbuffer);
            sensorToPoll = COLOR;
            break;
    }
#endif
#ifdef __MOTOR2680
    encoder_int_handler();
#endif
}


}