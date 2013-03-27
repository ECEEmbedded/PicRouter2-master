#include "maindefs.h"
#ifndef __XC8
#include <usart.h>
#include <timers.h>
#else
#include <plib/usart.h>
#endif
#include "my_uart.h"
#include "debug.h"

static uart_comm *uc_ptr;

void start_UART_send(unsigned char len, unsigned char * msg) {
    for (uc_ptr->outbuflen = 0; uc_ptr->outbuflen < len; ++uc_ptr->outbuflen) {
        uc_ptr->outbuffer[uc_ptr->outbuflen] = msg[uc_ptr->outbuflen];
    }

    uc_ptr->outbufind = 0;

#ifdef __USE18F26J50
    Write1USART(uc_ptr->outbuffer[0]);
#else
    PIE1bits.TXIE = 1;
#endif
}

static int a = 0;
void uart_recv_int_handler() {
    if (DataRdyUSART()) {
    int errs = RCSTA;

        uc_ptr->buffer[uc_ptr->buflen] = RCREG;

        INTCONbits.PEIE = 1;
        uc_ptr->buflen++;

        DebugPrint(a);
        a = !a;
        if (uc_ptr->buflen == MAXUARTBUF) {
            ToMainLow_sendmsg(uc_ptr->buflen, MSGT_UART_DATA, (void *) uc_ptr->buffer);
            uc_ptr->buflen = 0;
         };

         if (errs & 1) {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
        };
    }
}


void init_uart_snd_rcv(uart_comm *uc) {
    INTCONbits.PEIE = 1;
    PIE1bits.TXIE = 1;
    TXSTAbits.TXEN = 1;
    uc_ptr = uc;
    uc_ptr->buflen = 0;

//    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT &
//            USART_CONT_RX & USART_BRGH_LOW, 0x19);
    RCSTAbits.SPEN = 1;
    TRISCbits.TRISC7 = 1; // Tx = output pin
    TRISCbits.TRISC6 = 0; // Rx = input pin
    TXSTAbits.BRGH = 1;
    BAUDCONbits.BRG16 = 0;
    SPBRG = 0x65;
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    CREN = 1;

    RCIE = 1;;
}


void uart_send_int_handler(void) {
    if (uc_ptr->outbufind < uc_ptr->outbuflen) {
#ifdef __USE18F26J50
        Write1USART(uc_ptr->outbuffer[uc_ptr->outbufind]);
#else
        TXREG = uc_ptr->outbuffer[uc_ptr->outbufind];
#endif
        ++uc_ptr->outbufind;
    }
    else { // End of message
        PIE1bits.TXIE = 0;
    }
}

