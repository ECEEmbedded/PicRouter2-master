#include "i2cMaster.h"
#include "messages.h"
#include "my_i2c.h"
#include "debug.h"

enum {
    I2C_FREE,
    I2C_SENDING,
    I2C_REQUESTING,
    I2C_RECEIVING,
    I2C_RECEIVE_NEXT_BYTE,
    I2C_RECEIVED,
    I2C_REQUESTING_REG,
    I2C_ACKSTAT
};

typedef struct __i2c_master_comm {
	unsigned char buffer[MSGLEN];
	signed char     buflen;
        unsigned char   buffind;
	unsigned char event_count;
	unsigned char status;
	unsigned char error_code;
	unsigned char error_count;
} i2c_master_comm;

static i2c_master_comm i2c_p;

void i2c_master_start_next_in_Q(void);

// Sending in I2C Master mode [slave write]
// 		returns -1 if the i2c bus is busy
// 		return 0 otherwise
// Will start the sending of an i2c message -- interrupt handler will take care of
//   completing the message send.  When the i2c message is sent (or the send has failed)
//   the interrupt handler will send an internal_message of type MSGT_MASTER_SEND_COMPLETE if
//   the send was successful and an internal_message of type MSGT_MASTER_SEND_FAILED if the
//   send failed (e.g., if the slave did not acknowledge).  Both of these internal_messages
//   will have a length of 0.
// The subroutine must copy the msg to be sent from the "msg" parameter below into
//   the structure to which ic_ptr points [there is already a suitable buffer there].

unsigned char i2c_master_send(unsigned char adr, unsigned char length, unsigned char *msg) {
    unsigned char msgbuf[MSGLEN];
    msgbuf[0] = (adr << 1);
    int i;
    for (i = 0; i < length; ++i) {
        msgbuf[i+1] = msg[i];
    }
    FromMainHigh_sendmsg(length+1, MSGT_I2C_MASTER_SEND, msgbuf);

    i2c_master_start_next_in_Q();

    return 0;
}

// Receiving in I2C Master mode [slave read]
// 		returns 1 if the i2c bus is busy
// 		return 0 otherwise
// Will start the receiving of an i2c message -- interrupt handler will take care of
//   completing the i2c message receive.  When the receive is complete (or has failed)
//   the interrupt handler will send an internal_message of type MSGT_MASTER_RECV_COMPLETE if
//   the receive was successful and an internal_message of type MSGT_MASTER_RECV_FAILED if the
//   receive failed (e.g., if the slave did not acknowledge).  In the failure case
//   the internal_message will be of length 0.  In the successful case, the
//   internal_message will contain the message that was received [where the length
//   is determined by the parameter passed to i2c_master_recv()].
// The interrupt handler will be responsible for copying the message received into

unsigned char i2c_master_recv(unsigned char ID, char length) {
    unsigned char buf[2];
    buf[0] = (ID << 1)  | 0x01;
    buf[1] = length+1;  // +1 is so there is room for the i2c address
    FromMainHigh_sendmsg(2,MSGT_I2C_MASTER_RECV,buf);

    i2c_master_start_next_in_Q();

    return(0);
}

unsigned char i2c_master_request_reg(unsigned char ID, unsigned char adr, unsigned char length) {
    unsigned char buf[3];
    buf[0] = (ID << 1)  | 0x01;
    buf[1] = adr;
    buf[2] = length;
    FromMainHigh_sendmsg(3,MSGT_I2C_MASTER_REQUEST_REG,buf);
    i2c_master_start_next_in_Q();

    return(0);
}

// private function
void i2c_master_start_next_in_Q() {
    if (i2c_p.status != I2C_FREE) {
        return;
    }
    
    unsigned char msgType;
    i2c_p.buflen = FromMainHigh_recvmsg(MSGLEN, &msgType, i2c_p.buffer);

    if (i2c_p.buflen == MSGQUEUE_EMPTY) {
        return;
    }

    if (msgType == MSGT_I2C_MASTER_SEND) {
        i2c_p.buffind = 0;
        i2c_p.status = I2C_SENDING;
        SEN = 1;
    }
    else if (msgType == MSGT_I2C_MASTER_RECV) {

        i2c_p.buffind = 0;
        i2c_p.buflen = i2c_p.buffer[1];
        i2c_p.status = I2C_REQUESTING;
        SEN = 1;
    }
    else if (msgType == MSGT_I2C_MASTER_REQUEST_REG) {
        i2c_p.buffind = 0;
        i2c_p.buflen = i2c_p.buffer[2];
        i2c_p.status = I2C_REQUESTING_REG;
        SEN = 1;
    }
}


void i2c_master_int_handler() {
    switch (i2c_p.status) {
        case I2C_FREE: {
    DebugPrint(0x06);
            i2c_master_start_next_in_Q();
            break;
        }
        case I2C_SENDING: {
            if (i2c_p.buffind < i2c_p.buflen/* && !SSPCON2bits.ACKSTAT*/) {
                    SSPBUF = i2c_p.buffer[i2c_p.buffind];
                    i2c_p.buffind++;
                } else {    // we have nothing left to send
                    i2c_p.status = I2C_FREE;
                    PEN = 1;
                    //i2c_master_start_next_in_Q();
                }
            break;
        }
        case I2C_REQUESTING: {
DebugPrint(0x01);
            SSPBUF = i2c_p.buffer[i2c_p.buffind];
            ++i2c_p.buffind;
            i2c_p.status = I2C_ACKSTAT;
            break;
        }
        case I2C_RECEIVING: {
            i2c_p.buffer[i2c_p.buffind] = SSPBUF;
            ++i2c_p.buffind;

            if (i2c_p.buffind < i2c_p.buflen) {
                i2c_p.status = I2C_RECEIVE_NEXT_BYTE;
                ACKDT = 0;
DebugPrint(0x03);
            }
            else {    // we have nothing left to send
                i2c_p.status = I2C_RECEIVED;
DebugPrint(0x05);
                ACKDT = 1;
            }
            ACKEN = 1;
            break;
        }
        case I2C_RECEIVE_NEXT_BYTE: {
            i2c_p.status = I2C_RECEIVING;
DebugPrint(0x04);
            RCEN = 1;
            break;
        }
        case I2C_REQUESTING_REG: {
            if (i2c_p.buffind >= 2) {
                SSPBUF = i2c_p.buffer[i2c_p.buffind++];
            }
            else {
                i2c_p.buffind = 0;
                i2c_p.buffer[0] = i2c_p.buffer[0] & 0xFE;
                i2c_p.status = I2C_REQUESTING;
                RSEN = 1;
            }
            break;
        }
        case I2C_ACKSTAT: {
            if (SSPCON2bits.ACKSTAT) { // 1 if no acknowledge
                i2c_p.status = I2C_FREE;
                PEN = 1;
            }
            else {
DebugPrint(0x02);
                i2c_p.status = I2C_RECEIVING;
                RCEN = 1;
            }
            break;
        }
        case I2C_RECEIVED: {
            i2c_p.status = I2C_FREE;
DebugPrint(0x06);
            ToMainHigh_sendmsg(i2c_p.buflen, MSGT_I2C_DATA, i2c_p.buffer);
            PEN = 1;
        }
    }
}

/*
Function: I2CInit
Return:
Arguments:
Description: Initialize I2C in master mode, Sets the required baudrate
*/
void I2CInit(void){
    TRISC3 = 1;      /* SDA and SCL as input pin */
    TRISC4 = 1;      /* these pins can be configured either i/p or o/p */
    SSPSTAT |= 0x80; /* Slew rate disabled */
    SSPCON1 = 0x28;   /* SSPEN = 1, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1)) */
    SSPADD = 0x28;    /* 100Khz @ 4Mhz Fosc */

    i2c_p.status = I2C_FREE;
}
